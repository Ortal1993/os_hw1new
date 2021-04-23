cmake_minimum_required(VERSION 3.19)
project(hw1)

set(CMAKE_CXX_STANDARD 11)

add_executable(os_hw11new #main.cpp
        Commands.cpp Commands.h signals.cpp signals.h smash.cpp)