cmake_minimum_required(VERSION 3.6)
project(main)

set(CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE AUTOUIC ON)

find_package(OpenCV REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(main threadLabel.cpp main.cpp)

target_link_libraries(main PRIVATE Qt5::Core Qt5::Widgets ${OpenCV_LIBS})