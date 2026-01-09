cmake_minimum_required(VERSION 3.10)
project(Main)

set(CMAKE_CXX_STANDARD 20)

add_executable(Main main.cpp)

target_include_directories(Main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)
target_link_directories(Main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib)
target_link_libraries(Main PRIVATE Vector Number)