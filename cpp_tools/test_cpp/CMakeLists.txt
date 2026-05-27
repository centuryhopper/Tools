cmake_minimum_required(VERSION 3.20)

project(test_cpp
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)


# -------------------------
# Dependencies
# -------------------------
include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)

# -------------------------
# Main executable
# -------------------------

add_executable(app
    src/main.cpp
)


target_link_libraries(app PRIVATE fmt::fmt)


target_compile_options(app PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)


# -------------------------
# Tests
# -------------------------

enable_testing()

add_executable(tests
    tests/test_main.cpp
)

target_link_libraries(tests PRIVATE fmt::fmt)


add_test(NAME unit_tests COMMAND tests)

