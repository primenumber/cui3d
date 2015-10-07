cmake_minimum_required(VERSION 2.8)
project(CUI3D)

set(CUI3D_VERSION_MAJOR 0)
set(CUI3D_VERSION_MINOR 1)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -g -march=native -mtune=native -Wall -Wextra")
add_library(cui3d
  src/cui3d.cpp src/polygon.cpp src/geometry.cpp src/texture.cpp)
add_subdirectory(tests)
