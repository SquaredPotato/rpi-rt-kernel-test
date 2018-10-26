cmake_minimum_required(VERSION 3.6)
project(counter C)

include_directories (~/include ./include)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -lwiringPi)

add_executable(counter main.c)