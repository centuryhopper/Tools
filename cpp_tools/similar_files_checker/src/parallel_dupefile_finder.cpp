#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"
#include "../include/similar_files_checker/scoped_timer.hpp"
#include <algorithm>
#include <chrono> // chr::zoned_time, current_zone
#include <condition_variable>
#include <cstring> // strcasestr
#include <execution>
#include <expected>
#include <format>  // std::format
#include <fstream> // ifstream / ofstream
#include <mutex>
#include <print>
#include <ranges>
#include <thread>
#include <unordered_map>

namespace similar_files_checker {

namespace fs = std::filesystem; // the most widespread one; cppreference uses it
namespace rng = std::ranges;    // sometimes 'sr' or just 'ranges'
namespace rv = std::ranges::views;
namespace chr = std::chrono; // 'ch' and 'chrono' are also common

using namespace std::chrono_literals; // 10ms, 2s, 1h
using namespace std::string_literals; // "abc"s
using namespace std::literals;        // all std literal suffixes at once

std::vector<fs::path> walkDirectoryRecursively(const fs::path &path) {
  using namespace std;
  vector<fs::path> results, stack;
  stack.push_back(path);

  while (!stack.empty()) {
    fs::path parentPath = stack.back();
    stack.pop_back();
    if (fs::is_directory(parentPath)) {
      // std::println("{} is a directory", parentPath.string());
      // process children
      vector<fs::path> childDir;

      for (const auto &childPath : fs::directory_iterator(parentPath))
        childDir.push_back(childPath);

      // std::sort(childDir.begin(), childDir.end(), [](const fs::path& f1,
      // const fs::path& f2) {
      //     return f1.string() > f2.string();
      // });
      for (const auto &childPath : childDir)
        stack.push_back(childPath);
    } else {
      results.push_back(std::move(parentPath));
    }
  }

  return results;
}

std::vector<fs::path> walkDirectoryRecursivelyParallel(const fs::path &path,
                                                       uint numThreads) {
  using namespace std;

  std::error_code ec;
  if (fs::is_regular_file(path, ec)) {
    std::println(stderr, "please pass in a directory, not file.");
    return {};
  }

  std::mutex m;
  std::condition_variable cv;
  int shared_counter = 0;
  std::vector<fs::path> shared_results, shared_stack;
  // seed the root path to kick start the process
  shared_stack.push_back(path);
  bool areAllThreadsDone = false;

  auto threadWork = [&]() {
    while (true) {
      fs::path current;

      // Phase A: get work (locked)
      {
        std::unique_lock lock(m);

        cv.wait(lock,
                [&] { return !shared_stack.empty() || areAllThreadsDone; });

        if (areAllThreadsDone)
          return; // exit the thread: nothing popped, counter untouched

        current = std::move(shared_stack.back());
        shared_stack.pop_back();
        // std::println("currentDir: {}", current.string());
        ++shared_counter; // same locked region as the pop
      }

      // Phase B: read the directory (unlocked) so the speed up is here
      std::vector<fs::path> localDirs;
      std::vector<fs::path> localFiles;

      std::error_code ec;
      fs::directory_iterator it(current, ec);
      const fs::directory_iterator end;

      while (!ec && it != end) {
        const fs::directory_entry &entry = *it;

        std::error_code statusEc;
        const fs::file_status st = entry.symlink_status(statusEc);

        // if theres no error and path is not a symlink
        if (!statusEc && !fs::is_symlink(st)) {
          if (fs::is_directory(st)) {
            // println("current entry: {}", entry.path().string());
            localDirs.push_back(entry.path());
          } else if (fs::is_regular_file(st))
            localFiles.push_back(entry.path());
        }

        it.increment(ec); // always reached: one exit point per iteration
      }

      // Phase C: publish results (locked)
      {
        std::scoped_lock lock(m);

        const bool pushedWork = !localDirs.empty();

        // for (auto f: localFiles)
        //     std::println("{}", f.string());

        for (auto &dir : localDirs)
          shared_stack.push_back(std::move(dir));
        for (auto &file : localFiles)
          shared_results.push_back(std::move(file));

        // for (auto el: shared_stack)
        //     std::println("{}", el.string());

        --shared_counter; // pairs with the ++ in Phase A

        if (shared_stack.empty() && shared_counter == 0) {
          areAllThreadsDone = true;
          cv.notify_all(); // last one out: wake everyone so they can exit
        } else if (pushedWork) {
          cv.notify_all(); // new folders on the stack: wake sleepers to grab
                           // them
        }
      }
    }
  };

  // the block exists so threads join before the return
  {
    vector<jthread> threads;
    numThreads = std::min(std::thread::hardware_concurrency(), numThreads);
    println("numThreads: {}", numThreads);

    for (int i = 0; i < numThreads; i++) {
      threads.push_back(jthread(threadWork));
    }
  }

  return shared_results;
}

[[nodiscard]] std::expected<std::vector<fs::path>, std::string>
findSameSizeFiles(const std::vector<fs::path> &files) {
  using namespace std;

  struct SizedFile {
    uintmax_t size = 0;
    error_code ec;
    fs::path path;
  };

  vector<SizedFile> sized(files.size());

  ScopedTimer timer("findSameSizeFiles execution time");

  transform(execution::par, files.begin(), files.end(), sized.begin(),
            [](const fs::path &p) {
              SizedFile f{.path = p};
              f.size = fs::file_size(p, f.ec);
              return f;
            });

  // Report the first file whose size couldn't be read
  // if (auto it = rng::find_if(sized, [](const SizedFile& f) { return
  // static_cast<bool>(f.ec); });
  //     it != sized.end())
  // {
  //     return unexpected(
  //         FileSizeError{std::move(it->path), it->ec}
  //     );
  // }

  erase_if(sized, [](const SizedFile &entry) {
    return entry.size == 0 || static_cast<bool>(entry.ec);
  }); // C++20

  auto groupBySize = [&](std::vector<SizedFile> files)
      -> std::unordered_map<std::uintmax_t, std::vector<fs::path>> {
    std::unordered_map<uintmax_t, vector<fs::path>> groups;
    for (auto &[size, _, path] : files)
      groups[size].push_back(std::move(path));
    return groups;
  };

  auto map = groupBySize(sized);

  auto groups =
      map | rv::values |
      rv::filter([](const auto &g) { return g.size() > 1; }) | rv::join |
      rv::as_rvalue // moves values from map into result instead of copying
      | rng::to<vector<fs::path>>();

  return groups;
}

std::optional<std::uint64_t> partialHash(const fs::path &p, std::size_t n) {
  std::error_code ec;
  const auto size = fs::file_size(p, ec);
  if (ec)
    return std::nullopt;

  std::ifstream in(p, std::ios::binary);
  if (!in)
    return std::nullopt;

  std::string buf;
  if (size <= 2 * n) {
    // Small file: just hash the whole thing
    buf.resize(size);
    in.read(buf.data(), static_cast<std::streamsize>(size));
  } else {
    buf.resize(2 * n);
    in.read(buf.data(), static_cast<std::streamsize>(n));
    in.seekg(-static_cast<std::streamoff>(n), std::ios::end);
    in.read(buf.data() + n, static_cast<std::streamsize>(n));
  }
  if (!in)
    return std::nullopt;

  return XXH3_64bits(buf.data(), buf.size());
}

[[nodiscard]] std::expected<std::vector<fs::path>, std::string>
findSamePartialHashFiles(const std::vector<fs::path> &files) {
  using namespace std;

  struct PartialHashedFile {
    optional<uint64_t> hash = 0;
    fs::path path;
  };

  vector<PartialHashedFile> partialHashedFiles(files.size());

  transform(execution::par, files.begin(), files.end(),
            partialHashedFiles.begin(), [](const fs::path &p) {
              PartialHashedFile f{.path = p};
              f.hash = partialHash(p, 32);
              return f;
            });

  erase_if(partialHashedFiles, [](const PartialHashedFile &f) {
    return static_cast<bool>(f.hash == nullopt);
  }); // C++20

  std::unordered_map<uint64_t, vector<fs::path>> groups;
  for (auto &[hash, path] : partialHashedFiles)
    groups[*hash].push_back(std::move(path));

  auto result =
      groups | rv::values |
      rv::filter([](const auto &g) { return g.size() > 1; }) | rv::join |
      rv::as_rvalue // moves values from map into result instead of copying
      | rng::to<vector<fs::path>>();

  return result;
}

std::optional<Digest> fullHash(const fs::path &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in)
    return std::nullopt;

  blake3_hasher hasher;
  blake3_hasher_init(&hasher);

  std::vector<char> buf(1 << 16); // 64 KiB
  while (in) {
    in.read(buf.data(), static_cast<std::streamsize>(buf.size()));
    if (auto n = in.gcount(); n > 0)
      blake3_hasher_update(&hasher, buf.data(), static_cast<size_t>(n));
  }
  if (in.bad())
    return std::nullopt; // real read error, not just EOF

  Digest out;
  blake3_hasher_finalize(&hasher, out.data(), out.size());
  return out;
}

[[nodiscard]] std::expected<
    std::unordered_map<Digest, std::vector<fs::path>, DigestHash>, std::string>
findSameFullHashFiles(const std::vector<fs::path> &files) {
  using namespace std;

  struct FullHashedFile {
    optional<Digest> hash;
    fs::path path;
  };

  vector<FullHashedFile> fullHashedFiles(files.size());

  transform(execution::par, files.begin(), files.end(), fullHashedFiles.begin(),
            [&](const fs::path &p) {
              return FullHashedFile{
                  .hash = fullHash(p),
                  .path = p,
              };
            });

  erase_if(fullHashedFiles, [](const FullHashedFile &f) {
    return static_cast<bool>(f.hash == nullopt);
  }); // C++20

  std::unordered_map<Digest, vector<fs::path>, DigestHash> groups;
  for (auto &[hash, path] : fullHashedFiles)
    groups[*hash].push_back(std::move(path));

  std::erase_if(groups, [](const auto &entry) {
    const auto &[digest, paths] = entry;
    return paths.size() < 2;
  });

  return groups;
}

void handleDuplicates(const std::unordered_map<Digest, std::vector<fs::path>,
                                               DigestHash> &duplicates,
                      bool deleteFlag) {

  auto pathRank = [](const fs::path &path,
                     const std::vector<std::string> keywords) -> size_t {
    for (auto [i, kw] : rv::enumerate(keywords)) {
      if (strcasestr(path.c_str(), kw.c_str())) {
        return i;
      }
    }
    return keywords.size();
  };

  std::vector<std::string> priorityKeywords
      //{
      //    "dir5",
      //    "dir4",
      //};
      {
          "iphone/iphone_11_15_2023",
          "iphone",
          "le856501_export",
          "export",
  };

  const fs::path DIR_PATH =
      fs::path("/home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/"
               "tests/");

  auto now = chr::zoned_time{
      chr::current_zone(), chr::floor<chr::seconds>(chr::system_clock::now())};

  auto filename = std::format("logs/output_{:%Y-%m-%d_%H-%M-%S}.txt", now);

  const fs::path OUTPUT_PATH = DIR_PATH / filename;

  fs::create_directories(OUTPUT_PATH.parent_path());

  std::ofstream out(OUTPUT_PATH);
  for (const auto &[digestHash, paths] : duplicates) {
    if (paths.size() > 1) {

      // grab the path that takes the most priority which would contain a value
      // from the keywords array with the lowest index
      // the {} is the default std::ranges::less comparator
      const fs::path &keep = *rng::min_element(paths, {}, [&](const auto &p) {
        return std::pair{pathRank(p, priorityKeywords), p.native().size()};
      });
      std::println(out, "KEEP: {}", keep.string());

      for (const fs::path &p :
           paths | rv::filter([&](const auto &p) { return p != keep; })) {
        std::println(out, "{}", p.string());
        if (deleteFlag) {
          std::error_code ec;
          if (!fs::remove(p, ec)) {
            auto msg = ec ? ec.message() : "file not found";
            std::println(stderr, "Failed to delete {}: {}", p.string(), msg);
            std::println(out, "  ^ DELETE FAILED: {}", msg);
            // no return, no throw, so the loop carries on
          }
        }
      }

      std::println(out);
    }
  }
}

} // namespace similar_files_checker
