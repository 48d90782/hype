cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(hype)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 18)

list(APPEND _src src/main.cpp)

add_executable(hype ${_src})