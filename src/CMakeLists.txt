cmake_minimum_required(VERSION 2.8)
project(MCS)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS graph system filesystem REQUIRED)

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${LINK_DIRECTORIES} ${Boost_LIBRARY_DIRS})

add_executable(mcs mcs.cpp)
add_executable(modularProd modularProd.cpp)
add_executable(lineGraph lineGraph.cpp)
target_link_libraries(mcs ${Boost_LIBRARIES})
target_link_libraries(modularProd ${Boost_LIBRARIES})
target_link_libraries(lineGraph ${Boost_LIBRARIES})

