cmake_minimum_required(VERSION 3.5)
project(Iyokan-L2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(SOURCE_FILES main.cpp)
add_executable(main ${SOURCE_FILES})