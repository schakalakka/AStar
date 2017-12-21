cmake_minimum_required(VERSION 3.9.4)

project(astar)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11 -g -fPIC")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

set(SOURCE_FILES src/astar.c src/astar.h src/parser.c)

add_executable(astar ${SOURCE_FILES})