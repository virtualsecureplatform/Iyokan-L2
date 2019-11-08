include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(/usr/local/lib)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} source)
add_executable(main ${source})
target_link_libraries(main tfhe-spqlios-avx tbb pthread)
