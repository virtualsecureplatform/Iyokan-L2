cmake_minimum_required(VERSION 3.5)
project(Iyokan-L2)

link_directories(/usr/local/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Ofast -march=native")
set(SOURCE_FILES main.cpp)
add_executable(main ${SOURCE_FILES})
target_link_libraries(main tfhe-spqlios-avx tbb pthread)
