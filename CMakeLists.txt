cmake_minimum_required(VERSION 3.15)

project(Needle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(needle
    src/main.cpp
)

target_include_directories(needle PRIVATE include)