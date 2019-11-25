cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)
option(ARGPARSE_TEST_ENABLE "Build unit tests" ON)

set(ARGPARSE_VERSION "0.0.2")
project(argparse VERSION ${ARGPARSE_VERSION} LANGUAGES CXX)
enable_testing()

add_library(argparse INTERFACE)
target_include_directories(argparse INTERFACE .)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE argparse)

if(ARGPARSE_TEST_ENABLE)
    add_executable(tests tests.cpp)
    add_test(
        NAME tests
        COMMAND $<TARGET_FILE:tests>)
    target_link_libraries(tests PRIVATE argparse)
endif(ARGPARSE_TEST_ENABLE)