cmake_minimum_required(VERSION 3.10)

project(vfemu)

include_directories(${CMAKE_SOURCE_DIR}/include/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimplicit-fallthrough")

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
message("SDL2 Info: ")
message(STATUS "    DIR=${SDL2_DIR}")
message(STATUS "    INCLUDE_DIRS=${SDL2_INCLUDE_DIRS}")
message(STATUS "    LIBS=${SDL2_LIBS}")
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(src/)
add_subdirectory(modules/)
add_subdirectory(examples/)
