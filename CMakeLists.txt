cmake_minimum_required(VERSION 3.13)

project(mandelbrot)

set (CMAKE_CXX_STANDARD 14)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

#find_package(SFML 2.5 COMPONENTS system graphics audio window REQUIRED)

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -F ${FWKDIR} ${FWKS}")
add_subdirectory(src)
