cmake_minimum_required(VERSION 4.0)
project(Lab5Linux)

set(CMAKE_CXX_STANDARD 20)

add_executable(Lab5Linux main.cpp)
add_executable(ProcessM processM.cpp)
add_executable(ProcessA processA.cpp)
add_executable(ProcessP processP.cpp)
add_executable(ProcessS processS.cpp)
add_dependencies(Lab5Linux ProcessM ProcessA ProcessP ProcessS)