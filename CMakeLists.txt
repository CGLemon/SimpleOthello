cmake_minimum_required (VERSION 2.8)
project (SimpleOthello)
add_definitions(-std=c++14)
add_definitions(-O3)
aux_source_directory(./src DIR_SRCS)
add_executable(demo ${DIR_SRCS})

