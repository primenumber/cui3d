cmake_minimum_required(VERSION 2.8)
project(CUI3D)

set(CUI3D_VERSION_MAJOR 0)
set(CUI3D_VERSION_MINOR 1)

include_directories("${PROJECT_SOURCE_DIR}/include")
#add_subdirectory(include)
#add_subdirectory(src)
#add_subdirectory(tests)
set(CMAKE_CXX_FLAGS "-std=c++14 -O3 -g -march=native -mtune=native -Wall -Wextra")
add_library(cui3d
  src/cui3d.cpp src/polygon.cpp src/geometry.cpp src/texture.cpp)
add_executable(simple01 tests/simple01.cpp)
target_link_libraries(simple01 ncurses pthread boost_system boost_timer cui3d)
add_executable(simple02 tests/simple02.cpp)
target_link_libraries(simple02 ncurses pthread cui3d)
