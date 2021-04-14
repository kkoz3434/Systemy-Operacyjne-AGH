cmake_minimum_required(VERSION 3.16)
project(zad3 C)

set(CMAKE_C_STANDARD 99)

add_executable(zad3 sender.c)
add_executable(zad3.a catcher.c)