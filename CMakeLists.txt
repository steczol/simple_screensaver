cmake_minimum_required(VERSION 3.6)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create project
project(screensaver)

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(simple_screensaver src/simple_screensaver.cpp)
target_link_libraries(simple_screensaver ${OpenCV_LIBS})
