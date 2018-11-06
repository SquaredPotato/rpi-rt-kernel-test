cmake_minimum_required(VERSION 3.6)
project(timer C)

include_directories (~/include ./include)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -lwiringPi)

add_executable(timer main.c)