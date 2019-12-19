cmake_minimum_required(VERSION 3.5)
project(Iyokan-L2)

if(NOT IYOKANL2_ENABLE_CUFHE AND NOT IYOKANL2_ENABLE_TFHE)
    set(IYOKANL2_ENABLE_TFHE 1)
endif()

if(IYOKANL2_ENABLE_CUFHE)
    add_subdirectory(cufhe)
endif(IYOKANL2_ENABLE_CUFHE)

if(IYOKANL2_ENABLE_TFHE)
    add_subdirectory(tfhe)
endif(IYOKANL2_ENABLE_TFHE)
