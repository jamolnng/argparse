cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

set(ARGPARSE_VERSION "0.1.2")
project(argparse VERSION ${ARGPARSE_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(ARGPARSE_TEST_ENABLE "Build unit tests" ON)
option(ARGPARSE_BUILD_EXAMPLE "Build example" ON)
if(WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Wall /WX -Wno-c++98-compat -Wno-c++98-compat-pedantic \
            -Wno-gnu-zero-variadic-macro-arguments"
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Wall /WX")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(
            CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -pedantic-errors -Wall -Wextra \
            -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 \
            -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept \
            -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow \
            -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 \
            -Wswitch-default -Wundef -Werror -Wno-unused -Winline -Wconversion -Wfloat-equal\
            -Weffc++"
        )
    endif()
elseif(UNIX)
    set(
        CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -pedantic-errors -Wall -Wextra \
        -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 \
        -Winit-self -Wmissing-declarations -Wmissing-include-dirs \
        -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow \
        -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 \
        -Wswitch-default -Wundef -Werror -Wno-unused -Winline -Wconversion -Wfloat-equal\
        -Weffc++"
    )
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(
            CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op -Wnoexcept -Wstrict-null-sentinel"
        )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(
            CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments"
        )
    endif()
endif()

enable_testing()

add_library(argparse INTERFACE)
target_include_directories(argparse INTERFACE .)

if(ARGPARSE_BUILD_EXAMPLE)
    add_executable(example example.cpp)
    target_link_libraries(example PRIVATE argparse)
endif(ARGPARSE_BUILD_EXAMPLE)

if(ARGPARSE_TEST_ENABLE)
    add_executable(tests tests.cpp)
    add_test(
        NAME tests
        COMMAND $<TARGET_FILE:tests>)
    target_link_libraries(tests PRIVATE argparse)
endif(ARGPARSE_TEST_ENABLE)