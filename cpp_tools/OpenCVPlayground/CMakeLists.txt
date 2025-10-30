cmake_minimum_required(VERSION 3.10)
project(OpenCVPlayground VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(fmt REQUIRED)

set(SRC_FILES
    src/main.cpp
    src/virtual_painter.cpp
    src/document_scanner.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

# Register include folder
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} Eigen3::Eigen fmt::fmt)

