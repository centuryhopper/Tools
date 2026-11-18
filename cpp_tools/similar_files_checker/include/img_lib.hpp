#pragma once
#ifndef IMG_LIB_H
#define IMG_LIB_H

#include "../include/namespaces/namespaces.hpp"
#include "./similar_files_checker/phash_helpers.hpp"
#include "./similar_files_checker/union_find.hpp"
#include "./structures/ImageInfo.hpp"
#include <print>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
extern "C"
{
#include "../include/img_timestamp.h"
}
using Match = std::pair<std::size_t, std::size_t>;
std::vector<Match> findImgMatches(const std::vector<ImageInfo> &images,
                                  chr::seconds window);

std::optional<Timestamp> getImgCaptureTime(const fs::path &path);

bool isImage(const fs::path &path);
std::vector<ImageInfo> getImages(const fs::path &directory);
void groupImages(const fs::path &PARENT_PATH);
void deleteImgGroups(const fs::path &PARENT_PATH);

#endif
