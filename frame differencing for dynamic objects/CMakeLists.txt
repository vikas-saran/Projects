cmake_minimum_required(VERSION 3.10)
project(fd)
set(opencv_dir "C:/Users/vikassaran/Downloads/newcv/opencv/build/include")

# Find the OpenCV package
find_package(OpenCV REQUIRED PATHS ${opencv_dir})

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

# Executables for exercises
add_executable(fd main.cpp)
target_link_libraries(fd ${OpenCV_LIBS})