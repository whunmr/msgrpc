cmake_minimum_required(VERSION 3.6)
project(msgrpc)

LINK_DIRECTORIES(/Users/hmwang/lab/repo/googletest/googlemock/gtest)
INCLUDE_DIRECTORIES(/Users/hmwang/lab/repo/googletest/googletest/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/Users/hmwang/lab/thrift-0.9.3/lib/cpp/src)

set(SOURCE_FILES
    gen1/api_constants.cpp
    gen1/api_constants.h
    gen1/api_types.cpp
    gen1/api_types.h
    api.thrift
    main.cpp thrift/thrift_codec.h thrift/thrift_idl_declare.h demo_service.interface thrift/thrift_types.h thrift/thrift_idl_define.h thrift/thrift_idl_common.h demo_api_define.cpp demo_api_declare.h)

add_executable(msgrpc ${SOURCE_FILES})

target_link_libraries(msgrpc /Users/hmwang/lab/thrift-0.9.3/lib/cpp/.libs/libthrift.a gtest gtest_main)
