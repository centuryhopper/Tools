cmake_minimum_required(VERSION 3.10)
project(OpenCVPlayground VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(fmt REQUIRED)



add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} Eigen3::Eigen fmt::fmt)

