cmake_minimum_required(VERSION 3.10)

project(vfemu)

include_directories(${CMAKE_SOURCE_DIR}/include/)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_subdirectory(src/)
add_subdirectory(modules/)
add_subdirectory(examples/)
