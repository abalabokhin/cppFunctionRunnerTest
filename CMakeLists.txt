cmake_minimum_required(VERSION 2.8.4)
project(AnyFunctionRunner)

set(SOURCE_FILES main.cpp)
add_executable(AnyFunctionRunner ${SOURCE_FILES})

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)