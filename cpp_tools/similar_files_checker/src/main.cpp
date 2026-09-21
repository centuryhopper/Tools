#include <algorithm>
#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <ranges>
#include <string>

#include "../include/similar_files_checker/phash_helpers.hpp"
#include "../include/similar_files_checker/scoped_timer.hpp"
#include "../include/similar_files_checker/union_find.hpp"

// sudo apt install libopencv-dev libopencv-contrib-dev
#include <opencv2/opencv.hpp>
#include <opencv2/img_hash.hpp>

#include <exiv2/exiv2.hpp>

#include <iomanip>
#include <sstream>
#include <ctime>


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

*/

// const fs::path IMGS_PATH = "/home/leo_zhang/synology/le856501_export/iphone_11_15_2023/";
const fs::path IMGS_PATH = "/home/leo_zhang/synology/export/memories_backup/Takeout_6_20_2023/Google Photos/Photos from 2023/";



namespace fs = std::filesystem;

using Timestamp = std::chrono::system_clock::time_point;

Timestamp getCaptureTime(const fs::path& path);





struct ImageInfo {
    fs::path path;
    Timestamp timestamp;
    cv::Mat phash;
    // dimensions, filesize, etc.
};

bool isImage(const fs::path& path)
{
    std::string ext = path.extension().string();

    std::ranges::transform(ext, ext.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    return ext == ".jpg"  ||
           ext == ".jpeg" ||
           ext == ".png";
        //    ext == ".heic" ||
        //   ext == ".webp";
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
            .timestamp = getCaptureTime(entry.path()),
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

Timestamp getCaptureTime(const fs::path& path)
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

void logImgCreationDateTime(const std::string& PARENT_PATH)
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
const auto deleteGroups = [](const std::string& PARENT_PATH) -> void {
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

#include <omp.h>
#include <utility>

using Match = std::pair<std::size_t, std::size_t>;

std::vector<Match> findMatches(
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

    fmt::print("number of threads: {}\n", threadCount);

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

void groupImages(const std::string& PARENT_PATH)
{
    auto images = getImages(PARENT_PATH);

    fmt::print("Number of images: {}\n", images.size());

    const std::chrono::duration THRESHOLD = std::chrono::seconds{30};

    /*
        0–2: extremely similar / near-duplicate
        3–5: strong match candidate
        6–10: looser similarity, worth eyeballing
        >10: increasingly likely to be unrelated
    */
    std::unordered_map<int,std::vector<int>> imgGroups;
    custom_data_structures::UnionFind uf(images.size());

    // O (n * (n-1) / 2) comparisons in the worst case
    // for (int i=0;i<images.size();i++)
    // {
    //     for (int j=i+1;j<images.size();j++)
    //     {
    //         auto diff = images[j].timestamp - images[i].timestamp;

    //         // within 30 seconds
    //         if (diff <= THRESHOLD)
    //         {
    //             double distance =
    //             comparePHash(images[i].phash, images[j].phash);
    //             fmt::print(
    //                 "{} vs {} -> pHash distance: {}\n",
    //                 images[i].path.string(),
    //                 images[j].path.string(),
    //                 distance
    //             );

    //             // union the two image indices
    //             if (distance <= TOLERABLE_PHASH_DISTANCE)
    //             {
    //                 fmt::print("phash distance between {} and {}: {}", images[i].path.filename().string(), images[j].path.filename().string(), distance);
    //                 uf.unite(i, j);
    //             }
    //         }
    //         else
    //         {
    //             break;
    //         }
    //     }
    // }

    auto matches = findMatches(images, THRESHOLD);
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
    for (const auto& [root, indices] : imgGroups)
    {
        // fmt::print("root {} -> {}\n", root, indices);        

        fs::path uniqueFolder = IMGS_PATH / "unique_imgs";
        fs::create_directories(uniqueFolder);

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

        fs::path folderName = IMGS_PATH / std::format("group_{}", root);
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

//  make run ARGS="-d" add -d flag to keep first file for every folder starting with group_ and remove the rest
int main(int argc, char* argv[])
{
    // make sure argv is always -d, -v, -i, or -h
    /*
        -d = move first file (file to keep) from all group_ folders into parent directory then delete each group_ folder. This command naively keeps the first file for you and is useful if the number of group_ folders is large and you want to save time
        -v = group similar video files into their group_ folders
        -i = group similar img files into their group_ folders
        -h = show list of available commands
    */

    // fmt::print("argc: {}\n", argc);
    // fmt::print("argv[0]: {}\n", argv[0]);

    if (argc != 2)
    {
        displayInfo();
        return 1;
    }

    std::string arg = argv[1];
    std::unordered_set<std::string> s{"-d", "-v", "-i", "-h", "-l"};

    // std::string test = "a b c d e";
    // auto parts = test
    // | std::views::split(' ')
    // | std::views::transform([](auto&& r) {
    //       return std::string(r.begin(), r.end());
    //   })
    // | std::ranges::to<std::vector<std::string>>();
    // fmt::print("{}\n", parts);

    if (!s.contains(arg))
    {
        displayInfo();
        return 1;
    }

    switch (arg[1])
    {
        case 'd':
            deleteGroups(IMGS_PATH);
            break;
        case 'v':
            // TODO: work on video grouping logic (might scrap this feature tbh because I will eyeball my videos instead)
            break;
        case 'h':
            displayInfo();
            break;
        case 'i':
            {
                ScopedTimer timer("groupImages execution time");
                groupImages(IMGS_PATH);
                break;
            }
        case 'l':
            logImgCreationDateTime(IMGS_PATH);
            break;
        default:
            throw std::runtime_error("invalid command");
    }

    

    return 0;
}

