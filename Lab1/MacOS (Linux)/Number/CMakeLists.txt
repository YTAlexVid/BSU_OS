cmake_minimum_required(VERSION 3.31)
project(Number)

set(CMAKE_CXX_STANDARD 20)

add_library(Number STATIC Number.cpp
        Number.h
        NumberConstants.h)