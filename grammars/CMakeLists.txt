#+++
# 
# Copyright (c) 2022 JulesIMF, MIPT
#
# Module name:
#
#		CMakeLists.txt
#
# Abstract:
#
#		My default CMake. Allows to compile the project
#		changing only FILES (and other flags if needed).
#
#---

#
# Settings
#

cmake_minimum_required(VERSION 3.16)
project(formlangs_practask LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

#
# Definitions
#

# add_definitions(NDEBUG)

set(OPT "-O0")
set(SANITIZE 0)

if(SANITIZE)
    set(ASAN "-fsanitize=address")
else()
    set(ASAN "")
endif()

set(CMAKE_CXX_FLAGS "${OPT} ${WARNING_FLAGS} -g -fprofile-instr-generate -fcoverage-mapping")
	
#
# Building
#

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(includes)

# Earley

add_executable(earley
    src/grammars.cpp
    src/main_earley.cpp
    src/parsing/earley.cpp
)

target_link_libraries(earley)

# LR

# Test

add_executable(test
        src/grammars.cpp
        src/parsing/earley.cpp
        tests/main.cpp
        tests/grammar.cpp
        tests/earley.cpp
)

target_link_libraries(test gtest_main gtest pthread)