cmake_minimum_required(VERSION 3.21)
project(CALCULATOR C)

set(CMAKE_C_STANDARD 99)

add_executable(CALCULATOR main.c base_arithmetics.c base_arithmetics.h params.c params.h stackList.c stackList.h queueList.c queueList.h functions_module.c functions_module.h)
