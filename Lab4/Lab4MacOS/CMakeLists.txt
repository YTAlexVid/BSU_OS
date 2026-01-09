cmake_minimum_required(VERSION 4.0)
project(Lab4Linux)

add_executable(User user.cpp)
add_executable(Killer killer.cpp)
add_dependencies(User Killer)
set(CMAKE_CXX_STANDARD 20)
