#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <print>
#include <algorithm>
#include <system_error>
#include <thread>

namespace similar_files_checker {
    std::vector<fs::path> walkDirectoryRecursively(const fs::path &path)
    {
        using namespace std;
        vector<fs::path> results, stack;
        stack.push_back(path);
    
        while (!stack.empty())
        {
            fs::path parentPath = stack.back();
            stack.pop_back();
            if (fs::is_directory(parentPath))
            {
                // std::println("{} is a directory", parentPath.string());
                // process children
                vector<fs::path> childDir;
    
                for (const auto& childPath : fs::directory_iterator(parentPath))
                    childDir.push_back(childPath);
    
                // std::sort(childDir.begin(), childDir.end(), [](const fs::path& f1, const fs::path& f2) {
                //     return f1.string() > f2.string();
                // });
                for (const auto& childPath : childDir)
                    stack.push_back(childPath);
            }
            else
            {
                results.push_back(std::move(parentPath));
            }
        }
    
        return results;
    }


    std::vector<fs::path> walkDirectoryRecursivelyParallel(const fs::path& path)
    {
        using namespace std;

        std::error_code ec;
        if (fs::is_regular_file(path, ec))
        {
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
                
        // int numThreads = std::thread::hardware_concurrency();

        auto threadWork = [&]() {

            while (true)
            {
                fs::path current;

                // Phase A: get work (locked)
                {
                    std::unique_lock lock(m);
    
                    cv.wait(lock, [&] {
                        return !shared_stack.empty() || areAllThreadsDone;
                    });
    
                    if (areAllThreadsDone)
                        return;   // exit the thread: nothing popped, counter untouched
    
                    current = std::move(shared_stack.back());
                    shared_stack.pop_back();
                    // std::println("currentDir: {}", current.string());
                    ++shared_counter;   // same locked region as the pop
                }


                // Phase B: read the directory (unlocked)
                std::vector<fs::path> localDirs;
                std::vector<fs::path> localFiles;

                std::error_code ec;
                fs::directory_iterator it(current, ec);
                const fs::directory_iterator end;

                while (!ec && it != end)
                {
                    const fs::directory_entry& entry = *it;

                    std::error_code statusEc;
                    const fs::file_status st = entry.symlink_status(statusEc);

                    // if theres no error and path is not a symlink
                    if (!statusEc && !fs::is_symlink(st))
                    {
                        if (fs::is_directory(st))
                        {
                            // println("current entry: {}", entry.path().string());
                            localDirs.push_back(entry.path());
                        }
                        else if (fs::is_regular_file(st))
                            localFiles.push_back(entry.path());
                    }

                    it.increment(ec);   // always reached: one exit point per iteration
                }

                // Phase C: publish results (locked)
                {
                    std::scoped_lock lock(m);

                    const bool pushedWork = !localDirs.empty();

                    // for (auto f: localFiles)
                    //     std::println("{}", f.string());

                    for (auto& dir : localDirs)
                        shared_stack.push_back(std::move(dir));
                    for (auto& file : localFiles)
                        shared_results.push_back(std::move(file));

                    // for (auto el: shared_stack)
                    //     std::println("{}", el.string());

                    --shared_counter;   // pairs with the ++ in Phase A

                    if (shared_stack.empty() && shared_counter == 0)
                    {
                        areAllThreadsDone = true;
                        cv.notify_all();   // last one out: wake everyone so they can exit
                    }
                    else if (pushedWork)
                    {
                        cv.notify_all();   // new folders on the stack: wake sleepers to grab them
                    }
                }


            }

        };

        // the block exists so threads join before the return
        {
            vector<jthread> threads;
            const int numThreads = std::thread::hardware_concurrency();
            println("numThreads: {}", numThreads);

            for (int i=0;i<numThreads;i++)
            {
                threads.push_back(jthread(threadWork));
            }
        }

        



        return shared_results;
    }

}


