#include "vid_lib.hpp"
#include "structures/VideoHashCache.hpp"
#include "structures/VideoSimilarity.hpp"

Timestamp getVideoCaptureTime(const fs::path &path)
{
    AVFormatContext *ctx = nullptr;

    // Returns 0 on success. On failure ctx is left null, nothing to free.
    if (avformat_open_input(&ctx, path.c_str(), nullptr, nullptr) == 0)
    {
        std::optional<Timestamp> result;

        // 1. Container-level metadata — where most cameras write it.
        if (AVDictionaryEntry *tag =
                av_dict_get(ctx->metadata, "creation_time", nullptr, 0))
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
                    if (AVDictionaryEntry *tag =
                            av_dict_get(ctx->streams[i]->metadata,
                                        "creation_time", nullptr, 0))
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
        std::println(stderr, "Could not open video metadata for {}",
                     path.string());
    }

    // 3. Fall back to filesystem modification time.
    auto fileTime = fs::last_write_time(path);
    return fs::file_time_type::clock::to_sys(fileTime);
}

// Parses FFmpeg's "2023-06-20T14:03:12.000000Z" format (always UTC).
// Returns nullopt if the string doesn't parse.
std::optional<Timestamp> parseFFmpegTime(const char *value)
{
    std::tm tm{};
    std::istringstream ss(value);

    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S"); // stops at the '.' or 'Z'

    if (ss.fail())
        return std::nullopt;

    // timegm, not mktime: the tag is UTC, and mktime would apply your local
    // timezone offset on top of it.
    return chr::system_clock::from_time_t(timegm(&tm));
}

std::vector<Match> findVideoMatches(const std::vector<VideoInfo> &videos,
                                    chr::seconds window)
{
    const double MATCH_RATIO_THRESHOLD = 0.70; // 70% of frames must match

    // store each sample frames in a cache array
    std::vector<VideoHashCache> vfcs(videos.size());

    const int NUM_SAMPLES = 30;
    std::vector<bool> needsHash(videos.size(), false);
    for (std::size_t i = 0; i + 1 < videos.size(); ++i)
    {
        // if the next video is within the time window, mark both videos as
        // needing hashes
        if (videos[i + 1].timestamp - videos[i].timestamp <= window)
        {
            needsHash[i] = needsHash[i + 1] = true;
        }
    }

    std::mutex printMtx;
    tbb::parallel_for(0uz, videos.size(),
                      [&](auto &&i)
                      {
                          if (!needsHash[i])
                              return;
                          auto result = extractFrameHashes(
                              videos[i].path.string(), NUM_SAMPLES);

                          if (result)
                          {
                              vfcs[i] = std::move(*result);
                          }
                          else
                          {
                              // std::scoped_lock lock(printMtx); // unlocks at
                              // the end of this block fmt::println(stderr,
                              // "Skipping {}: {}", videos[i].path.string(),
                              //              result.error());
                          };
                      });

    tbb::concurrent_vector<Match> matches;
    tbb::parallel_for(0uz, videos.size(),
                      [&](auto &&i)
                      {
                          if (vfcs[i].hashes.empty())
                              return;

                          for (std::size_t j = i + 1; j < videos.size(); ++j)
                          {
                              const auto difference =
                                  videos[j].timestamp - videos[i].timestamp;

                              // images are sorted by timestamp.
                              if (difference > window)
                                  break;

                              if (vfcs[j].hashes.empty())
                                  continue;

                              VideoSimilarity sim =
                                  compareCachedHashes(vfcs[i], vfcs[j]);

                              if (sim.matchRatio >= MATCH_RATIO_THRESHOLD)
                              {
                                  matches.emplace_back(i, j);
                              }

                              // std::scoped_lock lock(printMtx);
                              // fmt::println("{} vs {} => comparable: {},
                              // matchRatio: {}",
                              //              videos[i].path.filename().string(),
                              //              videos[j].path.filename().string(),
                              //              sim.comparable, sim.matchRatio);
                          }
                      });

    return std::vector<Match>(matches.begin(), matches.end());
}

std::vector<VideoInfo> getVideos(const fs::path &directory)
{
    std::error_code ec;
    std::println("getting videos...");
    std::vector<VideoInfo> videos;

    for (fs::directory_iterator it(directory, ec), end; !ec && it != end;
         it.increment(ec))
    {
        auto entry = *it;
        // skip folders
        if (!entry.is_regular_file())
            continue;
        if (!isVideo(entry.path()))
            continue;

        VideoInfo video{{.path = entry.path(),
                         .timestamp = getVideoCaptureTime(entry.path())}};

        videos.push_back(std::move(video));
    }

    if (ec)
    {
        std::println(stderr, "vid error: {}", ec.message());
    }

    rng::sort(videos, [](const VideoInfo &a, const VideoInfo &b)
              { return a.timestamp < b.timestamp; });

    return videos;
}

void groupVideos(const fs::path &PARENT_PATH)
{
    auto videos = getVideos(PARENT_PATH);
    std::println("number of videos: {}", videos.size());
    const chr::duration WINDOW = chr::seconds{30};
    auto matches = findVideoMatches(videos, WINDOW);
    custom_data_structures::UnionFind uf(videos.size());
    std::unordered_map<int, std::vector<int>> videoGroups;
    videoGroups.reserve(videos.size());

    for (const auto &[i, j] : matches)
    {
        uf.unite(i, j);
    }

    for (size_t i = 0; i < videos.size(); i++)
    {
        int x = uf.find(i);
        videoGroups[x].push_back(i);
    }

    fs::path uniqueFolder = PARENT_PATH / "unique_vids";
    fs::create_directories(uniqueFolder);

    // Create folders based off of the roots of the union find data structure
    // and move each image their corresponding root folders
    for (const auto &[root, indices] : videoGroups)
    {
        // fmt::println("root {} -> {}", root, indices);

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

bool isVideo(const fs::path &path)
{
    std::string ext = path.extension().string();

    rng::transform(ext, ext.begin(), [](char c) { return std::tolower(c); });

    return ext == ".mp4" || ext == ".mov";
}

void deleteVideoGroups(const fs::path &PARENT_PATH)
{
    for (const auto &entry : fs::directory_iterator(PARENT_PATH))
    {
        if (!entry.is_directory())
            continue;

        const fs::path groupDir = entry.path();
        const std::string name = groupDir.filename().string();

        // only process folders starting with "group_"
        if (!name.starts_with("vid_group_"))
            continue;

        // find the first regular file
        for (const auto &item : fs::directory_iterator(groupDir))
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
                std::println("Skipping {}, destination already exists",
                             src.string());
            }

            break;
        }

        // removes the group folder AND everything remaining inside it
        fs::remove_all(groupDir);
    }
}
