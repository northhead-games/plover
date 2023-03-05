cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project("header_libs")

add_library(${PROJECT_NAME} INTERFACE include/vma/vk_mem_alloc.h)

target_include_directories(${PROJECT_NAME}
    INTERFACE include/
)