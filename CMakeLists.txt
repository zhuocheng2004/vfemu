cmake_minimum_required(VERSION 3.10)

project(vfemu)

include_directories(${CMAKE_SOURCE_DIR}/include/)

add_subdirectory(src/)
add_subdirectory(modules/)