cmake_minimum_required(VERSION 3.20)

project(similar_files_checker
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PROJECT_WARNINGS
    -Wall
    -Wextra
    -Wpedantic
)


include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    exiv2
    GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
    GIT_TAG v0.28.7
)

# Don't build stuff you don't need
set(EXIV2_BUILD_EXIV2_COMMAND OFF CACHE BOOL "" FORCE)
set(EXIV2_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(EXIV2_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(EXIV2_BUILD_DOC OFF CACHE BOOL "" FORCE)

# We don't need Brotli/JPEG XL support
set(EXIV2_ENABLE_BROTLI OFF CACHE BOOL "" FORCE)

set(EXIV2_ENABLE_INIH OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(exiv2)

target_include_directories(exiv2lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

# OpenCV
find_package(OpenCV REQUIRED)

# OpenMP
find_package(OpenMP REQUIRED)

# -------------------------
# Main executable
# -------------------------

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        include
        ${OpenCV_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_WARNINGS}
)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        fmt::fmt
        ${OpenCV_LIBS}
        Exiv2::exiv2lib
        OpenMP::OpenMP_CXX
)


# -------------------------
# Tests
# -------------------------

enable_testing()

add_executable(tests
    tests/test_main.cpp
)

target_include_directories(tests
    PRIVATE
        include
)

target_link_libraries(tests
    PRIVATE
        OpenMP::OpenMP_CXX
)

target_compile_options(tests
    PRIVATE
        ${PROJECT_WARNINGS}
)

add_test(NAME unit_tests COMMAND tests)



