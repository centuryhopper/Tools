#include <algorithm>
#include <cstring>
#include <exception>
#include <filesystem>
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <ranges>
#include <string>
#include <print>
#include <argparse/argparse.hpp>

#include "../include/similar_files_checker/phash_helpers.hpp"
#include "../include/similar_files_checker/scoped_timer.hpp"
#include "../include/similar_files_checker/union_find.hpp"

#include "../include/structures/ImageInfo.hpp"
#include "../include/structures/VideoInfo.hpp"

// sudo apt install libopencv-dev libopencv-contrib-dev
#include <opencv2/opencv.hpp>
#include <opencv2/img_hash.hpp>

#include <exiv2/exiv2.hpp>

#include <iomanip>
#include <sstream>
#include <ctime>

#include <omp.h>
#include <utility>


extern "C" {
    #include <libavformat/avformat.h>
    #include <libavutil/dict.h>
}


/*
scan images
      ↓
compute pHash
      ↓
sort by timestamp
      ↓
compare nearby images (sliding time window)
      ↓
pHash sufficiently similar?
      ↓ yes
union(i, j)
      ↓
finish all comparisons
      ↓
find() root of every image
      ↓
group images by root

fish syntax for converting .heic imgs to .jpg

set count 0
for i in *.HEIC
    set output (string replace -r '\.HEIC$' '.jpg' "$i")

    if heif-convert "$i" "$output"
        set count (math $count + 1)
    end
end
echo "Created $count JPG files"

=> make run ARGS="-i"

# delete all jpg, heic, and png files in current directory
=> find . -maxdepth 1 -type f \( -iname '*.json' -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' -o -iname '*.heic' \) -delete

# take first files in every group_ folder
=> make run ARGS="-d"

# count number of jpgs in current directory
find . -maxdepth 1 -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' -o -iname '*.heic' \) | count

# count videos in current directory
find . -maxdepth 1 -type f \( -iname '*.mov' -o -iname '*.mp4' \) | count

*/

// const fs::path IMGS_PATH = "/home/leo_zhang/synology/le856501_export/iphone_11_15_2023/";
// const fs::path IMGS_PATH = "/home/leo_zhang/synology/root/iphone/spring2024";
// const fs::path VIDS_PATH = "/home/leo_zhang/synology/root/iphone/spring2024";

namespace fs = std::filesystem;

using Timestamp = std::chrono::system_clock::time_point;

Timestamp getImgCaptureTime(const fs::path& path);
Timestamp getVideoCaptureTime(const fs::path& path);
using Match = std::pair<std::size_t, std::size_t>;


bool isImage(const fs::path& path)
{
    std::string ext = path.extension().string();

    std::ranges::transform(ext, ext.begin(), [](char c) {
        return std::tolower(c);
    });

    return ext == ".jpg"  ||
           ext == ".jpeg" ||
           ext == ".png";
        //    ext == ".heic" ||
        //   ext == ".webp";
}

bool isVideo(const fs::path& path)
{
    std::string ext = path.extension().string();

    std::ranges::transform(ext, ext.begin(), [](char c) {
        return std::tolower(c);
    });

    return ext == ".mp4" || ext == ".mov";
}

std::vector<ImageInfo> getImages(const fs::path& directory)
{
    fmt::print("getting images...\n");
    std::vector<ImageInfo> images;

    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (!entry.is_regular_file())
            continue;
        if (!isImage(entry.path()))
            continue;

        ImageInfo image {
            .path = entry.path(),
            .timestamp = getImgCaptureTime(entry.path()),
            .phash = computePHash(entry.path())
        };

        images.push_back(std::move(image));
    }

    std::sort(
        images.begin(),
        images.end(),
        [](const ImageInfo& a, const ImageInfo& b)
        {
            return a.timestamp < b.timestamp;
        });

    return images;
}

Timestamp getImgCaptureTime(const fs::path& path)
{
    try
    {
        auto image = Exiv2::ImageFactory::open(path.string());
        image->readMetadata();

        const auto& exif = image->exifData();

        auto it = exif.findKey(
            Exiv2::ExifKey("Exif.Photo.DateTimeOriginal")
        );

        if (it != exif.end())
        {
            std::tm tm{};
            std::istringstream ss(it->toString());

            ss >> std::get_time(&tm, "%Y:%m:%d %H:%M:%S");

            if (!ss.fail())
            {
                return std::chrono::system_clock::from_time_t(
                    std::mktime(&tm)
                );
            }
        }
    }
    catch (const Exiv2::Error& e)
    {
        fmt::print(
            stderr,
            "Could not read EXIF from {}: {}\n",
            path.string(),
            e.what()
        );
    }
    catch (const cv::Exception& e)
    {
        fmt::print(
            stderr,
            "OpenCV ERROR for {}: {}\n",
            path.string(),
            e.what()
        );
    }
    catch (const std::exception& e)
    {
        fmt::print(
            stderr,
            "ERROR for {}: {}\n",
            path.string(),
            e.what()
        );
    }

    // Fall back to filesystem modification time
    auto fileTime = fs::last_write_time(path);

    return decltype(fileTime)::clock::to_sys(fileTime);
}

void logImgCreationDateTime(const fs::path& PARENT_PATH)
{
    auto images = getImages(PARENT_PATH);
    for (const auto& image : images)
    {
        auto eastern = std::chrono::zoned_time {
            "America/New_York",
            image.timestamp
        };

        auto local = eastern.get_local_time();
        auto info = eastern.get_info();

        auto day = std::chrono::floor<std::chrono::days>(local);
        std::chrono::year_month_day date{day};

        auto time = std::chrono::hh_mm_ss{local - day};

        fmt::print(
            "{} -> {:04}-{:02}-{:02} {:02}:{:02}:{:02} {}\n",
            image.path.string(),
            int(date.year()),
            unsigned(date.month()),
            unsigned(date.day()),
            time.hours().count(),
            time.minutes().count(),
            time.seconds().count(),
            info.abbrev
        );
    }
}

// Loop thru all folders starting with group_  
// Move first item out of each group_ folder into parent directory and remove that group_ folder
void deleteGroups(const fs::path& PARENT_PATH) {
    for (const auto& entry : fs::directory_iterator(PARENT_PATH))
    {
        if (!entry.is_directory())
            continue;

        const fs::path groupDir = entry.path();
        const std::string name = groupDir.filename().string();

        // only process folders starting with "group_"
        if (!name.starts_with("group_"))
            continue;

        // find the first regular file
        for (const auto& item : fs::directory_iterator(groupDir))
        {
            if (!item.is_regular_file())
                continue;

            fs::path src = item.path();
            fs::path dst = PARENT_PATH / src.filename();

            // move first item back into parent directory
            if (!fs::exists(dst))
            {
                fs::rename(src, dst);
            }
            else
            {
                fmt::print("Skipping {}, destination already exists\n", src.string());
            }

            break;
        }

        // removes the group folder AND everything remaining inside it
        fs::remove_all(groupDir);
    }
};



std::vector<Match> findImgMatches(
    const std::vector<ImageInfo>& images,
    std::chrono::seconds window)
{
    const int TOLERABLE_PHASH_DISTANCE = 5;
    
    const auto isSimilar = [](const ImageInfo& img1, const ImageInfo& img2) -> bool
    {
        double distance = comparePHash(img1.phash, img2.phash);
        // fmt::print(
        //     "{} vs {} -> pHash distance: {}\n",
        //     img1.path.string(),
        //     img2.path.string(),
        //     distance
        // );

        // union the two image indices
        if (distance <= TOLERABLE_PHASH_DISTANCE)
        {
            // fmt::print("phash distance between {} and {}: {}", img1.path.filename().string(), img2.path.filename().string(), distance);
            return true;
        }

        return false;
    };

    // One vector for each possible OpenMP thread.
    const int threadCount = omp_get_max_threads();

    std::println("number of threads: {}", threadCount);

    std::vector<std::vector<Match>> localMatches(threadCount);

    #pragma omp parallel
    {
        const int threadId = omp_get_thread_num();

        // This thread owns this vector.
        auto& matches = localMatches[threadId];

        #pragma omp for schedule(dynamic)
        for (std::size_t i = 0; i < images.size(); ++i)
        {
            for (std::size_t j = i + 1; j < images.size(); ++j)
            {
                const auto difference =
                    images[j].timestamp - images[i].timestamp;

                // images are sorted by timestamp.
                if (difference > window)
                    break;

                if (isSimilar(images[i], images[j]))
                {
                    matches.emplace_back(i, j);
                }
            }
        }
    }

    // Combine all thread-local vectors.
    std::vector<Match> matches;

    for (const auto& local : localMatches)
    {
        matches.insert(
            matches.end(),
            local.begin(),
            local.end()
        );
    }

    return matches;
}

void groupImages(const fs::path& PARENT_PATH)
{
    auto images = getImages(PARENT_PATH);

    fmt::print("Number of images: {}\n", images.size());

    const std::chrono::duration WINDOW = std::chrono::seconds{30};

    /*
        0–2: extremely similar / near-duplicate
        3–5: strong match candidate
        6–10: looser similarity, worth eyeballing
        >10: increasingly likely to be unrelated
    */
    std::unordered_map<int,std::vector<int>> imgGroups;
    imgGroups.reserve(images.size());
    custom_data_structures::UnionFind uf(images.size());

    // O (n * (n-1) / 2) comparisons in the worst case for sequential approach


    auto matches = findImgMatches(images, WINDOW);
    for (const auto& [i,j] : matches)
    {
        uf.unite(i,j);
    }

    for (int i=0;i<images.size();i++)
    {
        int x = uf.find(i);
        imgGroups[x].push_back(i);
    }

    // Create folders based off of the roots of the union find data structure and move each image their corresponding root folders
    fs::path uniqueFolder = PARENT_PATH / "unique_imgs";

    fs::create_directories(uniqueFolder);

    for (const auto& [root, indices] : imgGroups)
    {
        // fmt::print("root {} -> {}\n", root, indices);        


        if (indices.size() == 1)
        {
            // fmt::print("only one img found\n");
            fs::path imgPath = images[indices[0]].path;
            fs::rename(imgPath, uniqueFolder / imgPath.filename());
            // fs::copy_file(
            //     imgPath,
            //     uniqueFolder / imgPath.filename(),
            //     fs::copy_options::skip_existing
            // );
            continue;
        }

        fs::path folderName = PARENT_PATH / std::format("group_{}", root);
        fs::create_directories(folderName);

        for (int idx : indices)
        {
            fs::path imgPath = images[idx].path;
            fs::rename(imgPath, folderName / imgPath.filename());
            // fs::copy_file(
            //     imgPath,
            //     folderName / imgPath.filename(),
            //     fs::copy_options::skip_existing
            // );
        }
    }
}

void displayInfo()
{
    fmt::print(R"(
        -d = move first file (file to keep) from all group_ folders into parent directory then delete each group_ folder. This command naively keeps the first file for you and is useful if the number of group_ folders is large and you want to save time
        -v = group similar video files into their group_ folders
        -i = group similar img files into their group_ folders
        -h = show list of available commands
        -l = log more information on each file
    )");
}

std::vector<VideoInfo> getVideos(const fs::path& PARENT_PATH)
{
    fmt::print("getting videos...\n");
    std::vector<VideoInfo> videos;
    for (const auto& file : fs::directory_iterator(PARENT_PATH))
    {
        // skip folders
        if (!file.is_regular_file()) continue;
        if (!isVideo(file.path())) continue;

        VideoInfo video {
            .path = file.path(),
            .timestamp = getVideoCaptureTime(file.path()),
        };

        videos.push_back(std::move(video));
    }

    std::sort(videos.begin(), videos.end(), [](const VideoInfo& a, const VideoInfo& b) {
        return a.timestamp < b.timestamp;
    });

    return videos;
}

std::vector<Match> findVideoMatches(const std::vector<VideoInfo>& videos, std::chrono::seconds window)
{
    const double MATCH_RATIO_THRESHOLD = 0.70;   // 70% of frames must match

    // store each sample frames in a cache array
    std::vector<VideoHashCache> vfcs(videos.size());
    const int threadCount = omp_get_max_threads();
    fmt::print("number of threads: {}\n", threadCount);

    std::vector<std::vector<Match>> localMatches(threadCount);
    std::vector<Match> matches;

    const int NUM_SAMPLES = 30;
    // const cv::Size TARGET_SIZE(320, 180);

    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < videos.size(); i++)
    {
        try
        {
            vfcs[i] = extractFrameHashes(videos[i].path.string(), NUM_SAMPLES).value();
        }
        catch (const std::exception& e)
        {
            // vfcs[i] stays empty, so the comparison loop skips it.
            #pragma omp critical
            fmt::print(stderr, "Skipping {}: {}\n", videos[i].path.string(), e.what());
        }
    }

    #pragma omp parallel
    {
        const int threadId = omp_get_thread_num();

        // This thread owns this vector.
        auto& localMatchVector = localMatches[threadId];

        #pragma omp for schedule(dynamic)
        for (std::size_t i = 0; i < videos.size(); ++i)
        {
            // no need to process a frameless video file
            if (vfcs[i].hashes.empty()) continue;

            for (std::size_t j = i + 1; j < videos.size(); ++j)
            {
                const auto difference =
                    videos[j].timestamp - videos[i].timestamp;

                // images are sorted by timestamp.
                if (difference > window)
                    break;

                if (vfcs[j].hashes.empty()) continue;

                VideoSimilarity sim = compareCachedHashes(vfcs[i], vfcs[j]);
                
                // #pragma omp critical
                // fmt::print("{} vs {} => comparable: {}, matchRatio: {}\n",
                //     videos[i].path.filename().string(),
                //     videos[j].path.filename().string(),
                //     sim.comparable,
                //     sim.matchRatio);

                if (sim.matchRatio >= MATCH_RATIO_THRESHOLD)
                {
                    localMatchVector.emplace_back(i, j);
                }
            }
        }
    }


    for (const auto& local : localMatches)
    {
        matches.insert(
            matches.end(),
            local.begin(),
            local.end()
        );
    }

    fmt::print("matches: {}\n", matches);


    return matches;
}

void groupVideos(const fs::path& PARENT_PATH)
{
    auto videos = getVideos(PARENT_PATH);
    fmt::print("number of videos: {}\n", videos.size());
    const std::chrono::duration WINDOW = std::chrono::seconds{30};
    auto matches = findVideoMatches(videos, WINDOW);
    custom_data_structures::UnionFind uf(videos.size());
    std::unordered_map<int,std::vector<int>> videoGroups;
    videoGroups.reserve(videos.size());

    for (const auto& [i,j] : matches)
    {
        uf.unite(i,j);
    }

    for (int i=0;i<videos.size();i++)
    {
        int x = uf.find(i);
        videoGroups[x].push_back(i);
    }

    fs::path uniqueFolder = PARENT_PATH / "unique_vids";
    fs::create_directories(uniqueFolder);

    // Create folders based off of the roots of the union find data structure and move each image their corresponding root folders
    for (const auto& [root, indices] : videoGroups)
    {
        // fmt::print("root {} -> {}\n", root, indices);        

        if (indices.size() == 1)
        {
            fs::path vidPath = videos[indices[0]].path;
            fs::rename(vidPath, uniqueFolder / vidPath.filename());
            continue;
        }

        fs::path folderName = PARENT_PATH / std::format("vid_group_{}", root);
        fs::create_directories(folderName);

        for (int idx : indices)
        {
            fs::path vidPath = videos[idx].path;
            fs::rename(vidPath, folderName / vidPath.filename());
        }
    }


}

// Parses FFmpeg's "2023-06-20T14:03:12.000000Z" format (always UTC).
// Returns nullopt if the string doesn't parse.
static std::optional<Timestamp> parseFFmpegTime(const char* value)
{
    std::tm tm{};
    std::istringstream ss(value);

    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");   // stops at the '.' or 'Z'

    if (ss.fail())
        return std::nullopt;

    // timegm, not mktime: the tag is UTC, and mktime would apply your local
    // timezone offset on top of it.
    return std::chrono::system_clock::from_time_t(timegm(&tm));
}


Timestamp getVideoCaptureTime(const fs::path& path)
{
    AVFormatContext* ctx = nullptr;

    // Returns 0 on success. On failure ctx is left null, nothing to free.
    if (avformat_open_input(&ctx, path.c_str(), nullptr, nullptr) == 0)
    {
        std::optional<Timestamp> result;

        // 1. Container-level metadata — where most cameras write it.
        if (AVDictionaryEntry* tag = av_dict_get(ctx->metadata, "creation_time", nullptr, 0))
        {
            result = parseFFmpegTime(tag->value);
        }

        // 2. Some files only put it on an individual stream.
        if (!result)
        {
            // Needed before the streams array is reliably populated.
            if (avformat_find_stream_info(ctx, nullptr) >= 0)
            {
                for (unsigned i = 0; i < ctx->nb_streams && !result; ++i)
                {
                    if (AVDictionaryEntry* tag =
                            av_dict_get(ctx->streams[i]->metadata, "creation_time", nullptr, 0))
                    {
                        result = parseFFmpegTime(tag->value);
                    }
                }
            }
        }

        // Always free what FFmpeg allocated, whether or not we found a tag.
        avformat_close_input(&ctx);

        if (result)
            return *result;
    }
    else
    {
        fmt::print(stderr, "Could not open video metadata for {}\n", path.string());
    }

    // 3. Fall back to filesystem modification time.
    auto fileTime = fs::last_write_time(path);
    return decltype(fileTime)::clock::to_sys(fileTime);
}

//  make run ARGS="-d" add -d flag to keep first file for every folder starting with group_ and remove the rest
int main(int argc, char* argv[])
{
    // so OpenCV doesn't spawn its own threads inside yours.
    cv::setNumThreads(0);

    // make sure argv is always -d, -v, -i, or -h
    /*
        -d = move first file (file to keep) from all group_ folders into parent directory then delete each group_ folder. This command naively keeps the first file for you and is useful if the number of group_ folders is large and you want to save time
        -v = group similar video files into their group_ folders
        -i = group similar img files into their group_ folders
        -h = show list of available commands
        -l = log more information on each file
    */

    argparse::ArgumentParser prog("Similar Files Checker", "1.0",
                              argparse::default_arguments::help);

    prog.add_argument("-img-path", "--img-path")
        .help("path to the directory containing images");

    prog.add_argument("-vid-path", "--vid-path")
        .help("path to the directory containing videos");

    // make sure only one of the following flags can be passed at a time
    auto& group = prog.add_mutually_exclusive_group(true);

    group.add_argument("-d", "--delete-imgs")
        .help("delete duplicate images")
        .flag();   // default false, true when passed

    group.add_argument("-v", "--videos")
        .help("group similar video files")
        .flag();

    group.add_argument("-i", "--images")
        .help("group similar image files")
        .flag();
    
    group.add_argument("-l", "--log")
        .help("log image creation dates")
        .flag();


    try {
        prog.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << '\n' << prog;
        return 1;
    }

    bool deleteImgsFlag  = prog.get<bool>("--delete-imgs");
    bool groupVideosFlag = prog.get<bool>("--videos");
    bool groupImagesFlag = prog.get<bool>("--images");
    bool logFlag         = prog.get<bool>("--log");

    bool needsImgs = groupImagesFlag || deleteImgsFlag || logFlag;
    bool needsVids = groupVideosFlag;

    const auto IMGS_PATH = prog.present<std::string>("--img-path");
    const auto VIDS_PATH = prog.present<std::string>("--vid-path");

    if (needsImgs && !IMGS_PATH) {
        std::cerr << "error: --img-path is required for -i, -d, and -l\n" << prog;
        return 1;
    }
    if (needsVids && !VIDS_PATH) {
        std::cerr << "error: --vid-path is required for -v\n" << prog;
        return 1;
    }

    if (deleteImgsFlag)
    {
        deleteGroups(*IMGS_PATH);
    }
    else if (groupVideosFlag)
    {
        ScopedTimer timer("groupVideos execution time");
        groupVideos(*VIDS_PATH);
    }
    else if (groupImagesFlag)
    {
        ScopedTimer timer("groupImages execution time");
        groupImages(*IMGS_PATH);
    }
    else if (logFlag)
    {
        logImgCreationDateTime(*IMGS_PATH);
    } 

    return 0;
}

/*
    finding and deleting exact duplicates (three passes):

        - recursively check all folders starting at root folder
            pass 1 dict[int, vector<paths>]:
                - group each files with the same file size (files of the same file size might be duplicates of each other)
                - filter out all value vectors with size of 1
                - flatten all values from dict to vector<paths>
            pass 2 dict[blake3::hash, vector<paths>]:
                - group each files with the same partial hash (files with different partial hashes are definitely unrelated)
                - filter out all value vectors with size of 1
                - flatten all values from dict to vector<paths>
            pass 3 dict[hash, vector<paths>] (at this point paths that still exist passed the first two checks so now we can go thru the trouble of fully hashing the files out and comparing them):
                - group each files with the same full hash
                - filter out all value vectors with size of 1
                - flatten all values from dict to vector<paths>
*/