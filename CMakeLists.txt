cmake_minimum_required(VERSION 3.20)
project(kbit VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_COVERAGE "Enable code coverage" OFF)
enable_testing()

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    klogger
    GIT_REPOSITORY https://github.com/IdanKoblik/klogger.git
    GIT_TAG main
)
FetchContent_MakeAvailable(klogger)

file(GLOB_RECURSE KBIT_LIB_SOURCES
    src/cmd/*.cpp
    src/parser/*.cpp
    src/file_handler.cpp
)

add_library(kbit_lib ${KBIT_LIB_SOURCES})
target_include_directories(kbit_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(kbit_lib
    PUBLIC
        klogger::klogger
)
target_compile_options(kbit_lib
    PRIVATE -Wall -Wextra -Wpedantic
)

add_executable(kbit src/main.cpp)
target_link_libraries(kbit PRIVATE kbit_lib)

file(GLOB_RECURSE KBIT_TEST_SOURCES tests/*.cpp)
add_executable(kbit_tests ${KBIT_TEST_SOURCES})
target_link_libraries(kbit_tests
    PRIVATE
        kbit_lib
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
)
include(GoogleTest)
gtest_discover_tests(kbit_tests)
