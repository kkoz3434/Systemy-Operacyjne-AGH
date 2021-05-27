cmake_minimum_required(VERSION 3.19)
project(zad1 C)

set(CMAKE_C_STANDARD 99)

add_executable(zad1 main.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")