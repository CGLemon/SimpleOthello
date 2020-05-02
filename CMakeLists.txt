cmake_minimum_required (VERSION 2.8)
project (SimpleOthello)
add_definitions(-std=c++14)
add_definitions(-O3)
find_package(Threads)

aux_source_directory(./src DIR_SRCS)
add_executable(SimpleOthello ${DIR_SRCS})
target_link_libraries(SimpleOthello ${CMAKE_THREAD_LIBS_INIT})
