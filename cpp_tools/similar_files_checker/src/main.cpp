#include <algorithm>
#include <argparse/argparse.hpp>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/parallel_for.h>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "../include/similar_files_checker/phash_helpers.hpp"
#include "../include/similar_files_checker/scoped_timer.hpp"

#include "../include/img_lib.hpp"
#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"
#include "../include/structures/ImageInfo.hpp"
#include "../include/structures/MediaInfo.hpp"
#include "../include/structures/VideoInfo.hpp"
#include "../include/vid_lib.hpp"

// sudo apt install libopencv-dev libopencv-contrib-dev
#include <opencv2/core/mat.hpp>
#include <opencv2/img_hash.hpp>

// #include <exiv2/exiv2.hpp>

#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <utility>

extern "C"
{
#include "../include/img_timestamp.h"
}

#include "../include/namespaces/namespaces.hpp"

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
=> find . -maxdepth 1 -type f \( -iname '*.json' -iname '*.jpg' -o -iname
'*.jpeg' -o -iname '*.png' -o -iname '*.heic' \) -delete

# take first files in every group_ folder
=> make run ARGS="-d"

# count number of jpgs in current directory
find . -maxdepth 1 -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname
'*.png' -o -iname '*.heic' \) | count

# count videos in current directory
find . -maxdepth 1 -type f \( -iname '*.mov' -o -iname '*.mp4' \) | count

*/

// const fs::path IMGS_PATH =
// "/home/leo_zhang/synology/le856501_export/iphone_11_15_2023/"; const fs::path
// IMGS_PATH = "/home/leo_zhang/synology/root/iphone/spring2024"; const fs::path
// VIDS_PATH = "/home/leo_zhang/synology/root/iphone/spring2024";

using namespace similar_files_checker;

void logFileCreationDateTime(const fs::path &IMG_PATH, const fs::path &VID_PATH)
{
    const auto &displayFileInfo = [](const MediaInfo &file)
    {
        auto eastern = chr::zoned_time{"America/New_York", file.timestamp};

        auto local = eastern.get_local_time();
        auto info = eastern.get_info();

        auto day = chr::floor<chr::days>(local);
        chr::year_month_day date{day};

        auto time = chr::hh_mm_ss{local - day};

        std::println("{} -> {:04}-{:02}-{:02} {:02}:{:02}:{:02} {}",
                     file.path.string(), int(date.year()),
                     unsigned(date.month()), unsigned(date.day()),
                     time.hours().count(), time.minutes().count(),
                     time.seconds().count(), info.abbrev);
    };

    auto images = getImages(IMG_PATH) | rng::to<std::vector<MediaInfo>>();
    auto vids = getVideos(VID_PATH) | rng::to<std::vector<MediaInfo>>();
    for (const auto &img : images)
    {
        displayFileInfo(img);
    }

    for (const auto &vid : vids)
    {
        displayFileInfo(vid);
    }
}

bool file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

/*
   make run ARGS="-d" add -d flag to keep first file for every folder starting
  with group_ and remove the rest
gdb --tui --args ./build/debug/bin/similar_files_checker -dedup --dedup-path
~/synology/root -keys 'iphone/iphone_11_15_2023,iphone,le856501_export,export'
-output-path /home/leo_zhang/Downloads
*/
/*
gdb --tui --args ./build/debug/bin/similar_files_checker -dedup -dedup-path
/home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/tests/directory_contents
-keys 'iphone/iphone_11_15_2023,iphone,le856501_export,export' -output-path
/home/leo_zhang/Downloads
*/
int main(int argc, char *argv[])
{
    // so OpenCV doesn't spawn its own threads inside yours.
    cv::setNumThreads(0);

    // char date[20];
    // if (argc < 2)
    // {
    //     perror("please add a path argument");
    //     return 1;
    // }
    // if (!(argv[1]))
    // {
    //     perror("directory not found");
    //     return 1;
    // }
    // bool found = get_img_timestamp(argv[1], date, sizeof date);
    // printf("%s\n", found ? "date found" : "date not found");
    // if (found)
    //     printf("date: %s\n", date);
    //
    // return 0;

    // make sure argv is always -d, -v, -i, or -h
    /*
        -di = delete duplicate images
        -dv = delete duplicate videos
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

    prog.add_argument("-dedup-path", "--dedup-path")
        .help("path to the directory containing duplicate files");

    prog.add_argument("-output-path", "--output-path")
        .help("path to the directory containing the output log file that "
              "displays the grouped duplicates and which one is kept (should "
              "only be used with dedup command)");

    prog.add_argument("-d", "--delete")
        .help("flag to determine whether to delete the duplicates")
        .flag();

    std::vector<std::string> keywords;
    prog.add_argument("-keys", "--keywords")
        .action(
            [&keywords](const std::string &s)
            {
                keywords = s | rv::split(',') |
                           rv::filter([](auto &&p) { return !p.empty(); }) |
                           rng::to<std::vector<std::string>>();
            })
        .help("priority folder names (delimited by commas) to determine which "
              "duplicates to keep (first value takes highest priority and last "
              "value takes lowest priority)");

    // make sure only one of the following flags can be passed at a time
    auto &group = prog.add_mutually_exclusive_group(true);

    group.add_argument("-di", "--delete-imgs")
        .help("delete similar images")
        .flag(); // default false, true when passed

    group.add_argument("-dv", "--delete-vids")
        .help("delete similar videos")
        .flag(); // default false, true when passed

    group.add_argument("-v", "--videos")
        .help("group similar video files")
        .flag();

    group.add_argument("-i", "--images")
        .help("group similar image files")
        .flag();

    group.add_argument("-l", "--log").help("log file creation dates").flag();

    group.add_argument("-dedup", "--dedup")
        .help("group and optionally delete duplicate files (recursively)")
        .flag();

    try
    {
        prog.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << '\n' << prog;
        return 1;
    }

    bool deleteImgsFlag = prog.get<bool>("--delete-imgs");
    bool deleteVidsFlag = prog.get<bool>("--delete-vids");
    bool groupVideosFlag = prog.get<bool>("--videos");
    bool groupImagesFlag = prog.get<bool>("--images");
    bool logFilesFlag = prog.get<bool>("--log");
    bool deDupFlag = prog.get<bool>("--dedup");
    bool deleteDupsFlag = prog.get<bool>("--delete");

    bool needsImgs = groupImagesFlag || deleteImgsFlag;
    bool needsVids = groupVideosFlag || deleteVidsFlag;
    bool needsDedupPath = deDupFlag;

    const auto IMGS_PATH = prog.present<std::string>("--img-path");
    const auto VIDS_PATH = prog.present<std::string>("--vid-path");
    const auto DEDUP_PATH = prog.present<std::string>("--dedup-path");
    const auto OUTPUT_PATH = prog.present<std::string>("--output-path");

    if (needsImgs && !IMGS_PATH)
    {
        std::cerr << "error: --img-path is required for -i, -di, and -l\n"
                  << prog;
        return 1;
    }
    if (needsVids && !VIDS_PATH)
    {
        std::cerr << "error: --vid-path is required for -v, -dv, and -l\n"
                  << prog;
        return 1;
    }
    if (needsDedupPath && (!DEDUP_PATH || !OUTPUT_PATH))
    {
        std::println(
            stderr,
            "error: --dedup-path and --output-path are required for -dedup");
        return 1;
    }

    if (deleteImgsFlag)
    {
        deleteImgGroups(IMGS_PATH.value_or(""));
    }
    else if (deleteVidsFlag)
    {
        deleteVideoGroups(VIDS_PATH.value_or(""));
    }
    else if (groupVideosFlag)
    {
        ScopedTimer timer("groupVideos execution time");
        groupVideos(VIDS_PATH.value_or(""));
    }
    else if (groupImagesFlag)
    {
        ScopedTimer timer("groupImages execution time");
        groupImages(IMGS_PATH.value_or(""));
    }
    else if (logFilesFlag)
    {
        logFileCreationDateTime(IMGS_PATH.value_or(""), VIDS_PATH.value_or(""));
    }
    else if (deDupFlag)
    {
        const int NUM_THREADS = 16;
        std::vector<fs::path> files = walkDirectoryRecursivelyParallel(
            DEDUP_PATH.value_or(""), NUM_THREADS);

        // std::println("files size: {}", files.size());

        ScopedTimer timer("Total execution time");

        return findSameSizeFiles(files)
            .and_then([](const std::vector<fs::path> &paths)
                      { return findSamePartialHashFiles(paths); })
            .and_then([](const std::vector<fs::path> &paths)
                      { return findSameFullHashFiles(paths); })
            .transform(
                [&](auto &&dupes)
                {
                    handleDuplicates(dupes, deleteDupsFlag, keywords,
                                     OUTPUT_PATH.value_or(""));
                    return 0; // success → exit code 0
                })
            .transform_error(
                [](auto &&err)
                {
                    std::println(stderr, "error: {}", err);
                    return 1;
                })
            .value_or(1); // error → exit code 1;
    }

    return 0;
}

/*
    finding and deleting exact duplicates (three passes):

        - recursively check all folders starting at root folder
            pass 1 dict[int, vector<paths>]:
                - group each files with the same file size (files of the same
   file size might be duplicates of each other)
                - filter out all value vectors with size of 1
                - flatten all values from dict to vector<paths>
            pass 2 dict[blake3::hash, vector<paths>]:
                - group each files with the same partial hash (files with
   different partial hashes are definitely unrelated)
                - filter out all value vectors with size of 1
                - flatten all values from dict to vector<paths>
            pass 3 dict[hash, vector<paths>] (at this point paths that still
   exist passed the first two checks so now we can go thru the trouble of fully
   hashing the files out and comparing them):
                - group each files with the same full hash
                - filter out all value vectors with size of 1
                - flatten all values from dict to vector<paths>
*/
