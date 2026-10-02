# CMake library snippets

Copy-paste lines for linking libraries. Replace `myapp` with your target name.

- `find_package(...)` goes near the top, after `project(...)`.
- `target_link_libraries(...)` goes after `add_executable(myapp ...)`.
- To remove a library: delete both lines.

## Finding the target name for a new library

After installing it, look at its CMake config files:

```bash
ls /usr/lib/x86_64-linux-gnu/cmake/ /usr/share/cmake/     # find the package folder
grep -rh "IMPORTED" /usr/lib/x86_64-linux-gnu/cmake/<Pkg>/  # target names are in here
```

The folder name (with its capitalization) is usually what goes in `find_package`.
Once it works, add it below.

---

## Formatting / logging / JSON

### fmt
apt: `libfmt-dev`
```cmake
find_package(fmt CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE fmt::fmt)
```

### spdlog
apt: `libspdlog-dev`
```cmake
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE spdlog::spdlog)
```

### nlohmann/json
apt: `nlohmann-json3-dev`
```cmake
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE nlohmann_json::nlohmann_json)
```

## CLI parsing

### CLI11 (fetched at configure time, no install needed)
```cmake
include(FetchContent)
FetchContent_Declare(cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG v2.4.2)
FetchContent_MakeAvailable(cli11)
target_link_libraries(myapp PRIVATE CLI11::CLI11)
```

## General purpose

### Boost (list the components you use)
apt: `libboost-all-dev` (or individual ones like `libboost-filesystem-dev`)
```cmake
find_package(Boost CONFIG REQUIRED COMPONENTS filesystem program_options)
target_link_libraries(myapp PRIVATE Boost::filesystem Boost::program_options)
```
Header-only Boost parts: `find_package(Boost CONFIG REQUIRED)` + link `Boost::headers`.

### Eigen
apt: `libeigen3-dev`
```cmake
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
target_link_libraries(myapp PRIVATE Eigen3::Eigen)
```

## Images / video

### OpenCV (list the modules you use)
apt: `libopencv-dev`
```cmake
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs videoio)
target_link_libraries(myapp PRIVATE opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio)
```

## Parallelism

### OpenMP
Comes with GCC. For Clang, apt: `libomp-dev`
```cmake
find_package(OpenMP REQUIRED)
target_link_libraries(myapp PRIVATE OpenMP::OpenMP_CXX)
```

### Threads (std::thread on Linux)
```cmake
find_package(Threads REQUIRED)
target_link_libraries(myapp PRIVATE Threads::Threads)
```

## Networking / data

### libcurl
apt: `libcurl4-openssl-dev`
```cmake
find_package(CURL REQUIRED)
target_link_libraries(myapp PRIVATE CURL::libcurl)
```

### OpenSSL
apt: `libssl-dev`
```cmake
find_package(OpenSSL REQUIRED)
target_link_libraries(myapp PRIVATE OpenSSL::SSL OpenSSL::Crypto)
```

### SQLite
apt: `libsqlite3-dev`
```cmake
find_package(SQLite3 REQUIRED)
target_link_libraries(myapp PRIVATE SQLite::SQLite3)
```

### zlib
apt: `zlib1g-dev`
```cmake
find_package(ZLIB REQUIRED)
target_link_libraries(myapp PRIVATE ZLIB::ZLIB)
```

## Testing (link to your test target, not the app)

### Catch2 v3
apt: `catch2` (v3 on Ubuntu 24.04-based distros; v2 on older ones uses `Catch2::Catch2` and needs your own main)
```cmake
find_package(Catch2 3 REQUIRED)
add_executable(tests tests/test_main.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
include(CTest)
include(Catch)
catch_discover_tests(tests)
```

### GoogleTest
apt: `libgtest-dev`
```cmake
find_package(GTest REQUIRED)
add_executable(tests tests/test_main.cpp)
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)
include(CTest)
include(GoogleTest)
gtest_discover_tests(tests)
```

---

## Template for new entries

### <name>
apt: `<package>`
```cmake
find_package(<Pkg> CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE <Pkg>::<target>)
```
