cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
project(hype LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(DEFAULT_LINKER_OPTIONS -pthread)
find_package(Threads REQUIRED)
find_package(Boost 1.73 COMPONENTS program_options REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

file(
        GLOB
        _src
        src/*.cpp
        include/*.h
)

#list(APPEND _src
#        src/main.cpp
#        src/types.h)

add_executable(hype ${_src})
target_link_libraries(hype Catch2::Catch2 ${DEFAULT_LINKER_OPTIONS} ${Boost_LIBRARIES})