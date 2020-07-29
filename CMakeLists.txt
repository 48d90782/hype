cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(hype)

list(APPEND _src src/main.cpp)

add_executable(hype ${_src})