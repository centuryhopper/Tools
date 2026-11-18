#pragma once
#ifndef VID_LIB_H
#define VID_LIB_H

#include "../include/namespaces/namespaces.hpp"
#include "./similar_files_checker/phash_helpers.hpp"
#include "./similar_files_checker/union_find.hpp"
#include "./structures/VideoInfo.hpp"
#include <mutex>
#include <print>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>

using Match = std::pair<std::size_t, std::size_t>;
bool isVideo(const fs::path &path);

void deleteVideoGroups(const fs::path &PARENT_PATH);
std::vector<VideoInfo> getVideos(const fs::path &directory);

Timestamp getVideoCaptureTime(const fs::path &path);
std::optional<Timestamp> parseFFmpegTime(const char *value);
std::vector<Match> findVideoMatches(const std::vector<VideoInfo> &videos,
                                    chr::seconds window);
void groupVideos(const fs::path &PARENT_PATH);

bool isVideo(const fs::path &path);

#endif
