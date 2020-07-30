cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(hype LANGUAGES C CXX)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 18)

set(DEFAULT_LINKER_OPTIONS -pthread)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

if (Boost_FOUND)
    message("version: " ${Boost_VERSION})
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

# TESTS
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v2.13.0
)
FetchContent_MakeAvailable(catch2)

if (NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
endif ()

list(APPEND _src
        src/main.cpp
        src/types.h)

add_executable(hype ${_src})
target_link_libraries(hype Catch2 ${DEFAULT_LINKER_OPTIONS} ${Boost_LIBRARIES})