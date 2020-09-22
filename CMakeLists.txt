cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(hype LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

IF(WIN32)
    message("windows platform detected")
ELSE()
    message("linux platform detected")
set(DEFAULT_LINKER_OPTIONS -pthread)
ENDIF()

find_package(Threads REQUIRED)

find_package(Boost 1.73 REQUIRED COMPONENTS program_options iostreams)
find_package(ZLIB REQUIRED)
find_package(GTest CONFIG REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

file(
        GLOB
        _src
        src/main.cpp
        src/decode.h
        src/decoder.cpp
        src/profile.h
        src/profile.cpp
        include/*.h
)

add_library(hype STATIC ${_src})
target_link_libraries(hype PRIVATE ${DEFAULT_LINKER_OPTIONS} ${Boost_LIBRARIES} ZLIB::ZLIB)

add_executable(hype_tests src/decoder_test.cpp src/decoder.h)
target_link_libraries(hype_tests PRIVATE hype ${DEFAULT_LINKER_OPTIONS} GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

add_executable(hype_run ${_src})
target_link_libraries(hype_run ${DEFAULT_LINKER_OPTIONS} ${Boost_LIBRARIES} ZLIB::ZLIB)