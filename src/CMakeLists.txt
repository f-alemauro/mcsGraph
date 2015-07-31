cmake_minimum_required(VERSION 2.8)
project(MCS)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.38 COMPONENTS required)

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${LINK_DIRECTORIES} ${Boost_LIBRARY_DIRS})

add_executable(mcs mcs.cpp)
target_link_libraries(mcs boost_graph)

