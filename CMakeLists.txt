cmake_minimum_required(VERSION 3.10)
project(MetaPascal)

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.*)

add_executable(MetaPascal ${SRC})