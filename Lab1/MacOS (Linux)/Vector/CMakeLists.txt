cmake_minimum_required(VERSION 3.31)
project(Vector)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")

add_library(Vector SHARED Vector.cpp
        Vector_Export.h
        Vector.h
        VectorConstants.cpp
        VectorConstants.h)

target_include_directories(Vector PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)

target_link_directories(Vector PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib
)

target_link_libraries(Vector PRIVATE
        Number
)