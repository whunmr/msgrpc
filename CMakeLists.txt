cmake_minimum_required(VERSION 3.3)
project(msgrpc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#separate libraries depended by msgrpc and testcases
LINK_DIRECTORIES(/Users/hmwang/lab/repo/googletest/googlemock/gtest)
INCLUDE_DIRECTORIES(/Users/hmwang/lab/repo/googletest/googletest/include)

#using boost to make udp sockets for testcases to simulate IPC
LINK_DIRECTORIES(/usr/local/Cellar/boost/1.60.0_1/lib)
INCLUDE_DIRECTORIES(/usr/local/Cellar/boost/1.60.0_1/include)


LINK_DIRECTORIES(/Users/hmwang/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/lib/)
INCLUDE_DIRECTORIES(/Users/hmwang/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/include/)
add_definitions(-DTHREADED)

include_directories(/Users/hmwang/lab/thrift-0.9.3/lib/cpp/src)
include_directories(/Users/hmwang/lab/cpp/msgrpc)
include_directories(/Users/hmwang/lab/cpp/msgrpc/include)

set(SOURCE_FILES
        demo/gen1/api_constants.cpp
        demo/gen1/api_constants.h
        demo/gen1/api_types.cpp
        demo/gen1/api_types.h
        demo/gen1/api.thrift
        test/test_thrift_struct.cpp include/msgrpc/thrift_struct/thrift_codec.h include/msgrpc/thrift_struct/thrift_struct_declare.h demo/demo_service.interface include/msgrpc/thrift_struct/thrift_types.h include/msgrpc/thrift_struct/thrift_struct_define.h include/msgrpc/thrift_struct/thrift_struct_common.h demo/demo_api_define.cpp demo/demo_api_declare.h include/msgrpc/api/service_api_declare.h include/msgrpc/api/service_api_define.h test/test_zookeeper.cpp test/test_async_rpc.cpp test/test_util/UdpChannel.h include/msgrpc/util/singleton.h)

add_executable(msgrpc ${SOURCE_FILES})

target_link_libraries(msgrpc /Users/hmwang/lab/thrift-0.9.3/lib/cpp/.libs/libthrift.a gtest gtest_main zookeeper_mt boost_system)
