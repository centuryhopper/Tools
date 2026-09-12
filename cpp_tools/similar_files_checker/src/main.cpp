#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>
#include <chrono>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>

#include "../include/similar_files_checker/phash_helpers.hpp"
#include "../include/similar_files_checker/union_find.hpp"

// sudo apt install libopencv-dev libopencv-contrib-dev
#include <opencv2/opencv.hpp>
#include <opencv2/img_hash.hpp>

#include <exiv2/exiv2.hpp>

#include <iomanip>
#include <sstream>
#include <ctime>


namespace fs = std::filesystem;

using Timestamp = std::chrono::system_clock::time_point;

Timestamp getCaptureTime(const fs::path& path);


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
for i in *.HEIC
    heif-convert "$i" (string replace -r '\.HEIC$' '.jpg' "$i")
end
*/


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
           ext == ".png"  ||
           ext == ".heic" ||
           ext == ".webp";
}

std::vector<ImageInfo> getImages(const fs::path& directory)
{
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
    auto image = Exiv2::ImageFactory::open(path.string());
    image->readMetadata();

    const auto& exif = image->exifData();
    auto it = exif.findKey(Exiv2::ExifKey("Exif.Photo.DateTimeOriginal"));

    if (it != exif.end())
    {
        std::tm tm{};
        std::istringstream ss(it->toString());

        ss >> std::get_time(&tm, "%Y:%m:%d %H:%M:%S");

        if (!ss.fail())
            return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    auto fileTime = fs::last_write_time(path);
    return decltype(fileTime)::clock::to_sys(fileTime);
}


int main() {
    auto images = getImages("/home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/src/images");

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
    for (const auto& image : images)
    {
        // if (i==10) break;
        std::cout << image.path << '\n';
        // i+=1;
    }

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

    // TODO: create folders based off of the roots of the union find data structure and move each image their corresponding root folders

    for (const auto& [root, indices] : imgGroups)
    {
        fmt::print("root {} -> {}\n", root, indices);
    }

    

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

    return 0;
}

