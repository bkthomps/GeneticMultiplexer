cmake_minimum_required(VERSION 3.17)
project(GeneticMultiplexer)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wpedantic")

add_executable(GeneticMultiplexer src/main.cpp src/expressions.h src/expressions.cpp src/constants.h)