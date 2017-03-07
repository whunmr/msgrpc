cmake_minimum_required(VERSION 3.6)
project(msgrpc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/Users/hmwang/lab/thrift-0.9.3/lib/cpp/src)

set(SOURCE_FILES
    gen1/api_constants.cpp
    gen1/api_constants.h
    gen1/api_types.cpp
    gen1/api_types.h
    api.thrift
    LICENSE
    README.md main.cpp)

add_executable(msgrpc ${SOURCE_FILES})