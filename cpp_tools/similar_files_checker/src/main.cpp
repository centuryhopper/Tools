#include <algorithm>
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

=> make run

# delete all jpg, heic, and png files in current directory
=> find . -maxdepth 1 -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' -o -iname '*.heic' \) -delete

# take first files in every group_ folder
=> make run ARGS="-d"

# count number of jpgs in current directory
find . -maxdepth 1 -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' -o -iname '*.heic' \) | count

*/

const fs::path IMGS_PATH = "/home/leo_zhang/synology/iphone/summer2024";



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

void logImgCreationDateTime(std::vector<ImageInfo> images)
{
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
const auto deleteGroups = []() -> void {
    for (const auto& entry : fs::directory_iterator(IMGS_PATH))
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
            fs::path dst = IMGS_PATH / src.filename();

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

void groupImages()
{
    auto images = getImages(IMGS_PATH);

    fmt::print("Number of images: {}\n", images.size());

    const std::chrono::duration THRESHOLD = std::chrono::seconds{30};

    /*
        0–2: extremely similar / near-duplicate
        3–5: strong match candidate
        6–10: looser similarity, worth eyeballing
        >10: increasingly likely to be unrelated
    */
    const int TOLERABLE_PHASH_DISTANCE = 5;

    std::unordered_map<int,std::vector<int>> imgGroups;
    custom_data_structures::UnionFind uf(images.size());

    int i=0;
    // for (const auto& image : images)
    // {
    //     // if (i==10) break;
    //     std::cout << image.path << '\n';
    //     // i+=1;
    // }


    // O (n * (n-1) / 2) comparisons in the worst case
    for (int i=0;i<images.size();i++)
    {
        for (int j=i+1;j<images.size();j++)
        {
            auto diff = images[j].timestamp - images[i].timestamp;

            // within 30 seconds
            if (diff <= THRESHOLD)
            {
                double distance =
                comparePHash(images[i].phash, images[j].phash);
                fmt::print(
                    "{} vs {} -> pHash distance: {}\n",
                    images[i].path.string(),
                    images[j].path.string(),
                    distance
                );

                // union the two image indices
                if (distance <= TOLERABLE_PHASH_DISTANCE)
                {
                    fmt::print("phash distance between {} and {}: {}", images[i].path.filename().string(), images[j].path.filename().string(), distance);
                    uf.unite(i, j);
                }
            }
            else
            {
                break;
            }
        }
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
            fs::copy_file(
                imgPath,
                uniqueFolder / imgPath.filename(),
                fs::copy_options::skip_existing
            );
            continue;
        }

        fs::path folderName = IMGS_PATH / std::format("group_{}", root);
        fs::create_directories(folderName);

        for (int idx : indices)
        {
            fs::path imgPath = images[idx].path;
            fs::copy_file(
                imgPath,
                folderName / imgPath.filename(),
                fs::copy_options::skip_existing
            );
        }

    }

    
    

    
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

    if (argc > 2)
    {
        fmt::print(R"(
            -d = move first file (file to keep) from all group_ folders into parent directory then delete each group_ folder. This command naively keeps the first file for you and is useful if the number of group_ folders is large and you want to save time
            -v = group similar video files into their group_ folders
            -i = group similar img files into their group_ folders
            -h = show list of available commands)");
        return 1;
    }

    std::string arg = argv[1];
    std::unordered_set<std::string> s{"-d", "-v", "-i", "-h"};

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
        fmt::print(R"(
            -d = move first file (file to keep) from all group_ folders into parent directory then delete each group_ folder. This command naively keeps the first file for you and is useful if the number of group_ folders is large and you want to save time
            -v = group similar video files into their group_ folders
            -i = group similar img files into their group_ folders
            -h = show list of available commands)");
        return 1;
    }

    switch (arg[1])
    {
        case 'd':
            deleteGroups();
            break;
        case 'v':
            // TODO: work on video grouping logic
            break;
        case 'h':
            fmt::print(R"(
                -d = move first file (file to keep) from all group_ folders into parent directory then delete each group_ folder. This command naively keeps the first file for you and is useful if the number of group_ folders is large and you want to save time
                -v = group similar video files into their group_ folders
                -i = group similar img files into their group_ folders
                -h = show list of available commands)");
            break;
        case 'i':
            groupImages();
            break;
        default:
            throw std::runtime_error("invalid command");
    }

    

    return 0;
}

