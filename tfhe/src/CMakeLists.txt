include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../picojson)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(/usr/local/lib)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} source)
add_executable(iyokanl2 ${source})
target_link_libraries(iyokanl2 tfhe-spqlios-avx tbb pthread)
