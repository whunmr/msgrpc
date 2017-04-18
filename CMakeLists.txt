cmake_minimum_required(VERSION 3.3)
project(msgrpc)

################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

################################################################################
#separate libraries depended by msgrpc and testcases
LINK_DIRECTORIES(/Users/hmwang/lab/repo/googletest/googlemock/gtest)
INCLUDE_DIRECTORIES(/Users/hmwang/lab/repo/googletest/googletest/include)

#using boost to make udp sockets for testcases to simulate IPC
LINK_DIRECTORIES(/usr/local/Cellar/boost/1.60.0_1/lib)
INCLUDE_DIRECTORIES(/usr/local/Cellar/boost/1.60.0_1/include)


LINK_DIRECTORIES(/Users/hmwang/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/lib/)
INCLUDE_DIRECTORIES(/Users/hmwang/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/include/)
add_definitions(-DTHREADED)

################################################################################
include_directories(/Users/hmwang/lab/thrift-0.9.3/lib/cpp/src)
include_directories(/Users/hmwang/lab/cpp/msgrpc/include)
include_directories(/Users/hmwang/lab/cpp/msgrpc)

################################################################################
set(SOURCE_FILES
        demo/demo_api_declare.h
        demo/demo_api_define.cpp
        demo/demo_service.interface
        demo/gen1/api.thrift
        demo/gen1/api_constants.cpp
        demo/gen1/api_constants.h
        demo/gen1/api_types.cpp
        demo/gen1/api_types.h
        include/msgrpc/api/service_api_declare.h
        include/msgrpc/api/service_api_define.h
        include/msgrpc/core/rpc_sequence_id.h
        include/msgrpc/core/cell/cell_base.h
        include/msgrpc/thrift_struct/thrift_codec.h
        include/msgrpc/thrift_struct/thrift_struct_common.h
        include/msgrpc/thrift_struct/thrift_struct_declare.h
        include/msgrpc/thrift_struct/thrift_struct_define.h
        include/msgrpc/thrift_struct/thrift_types.h
        include/msgrpc/util/singleton.h
        include/msgrpc/util/type_traits.h
        test/test_async_rpc.cpp
        test/test_thrift_struct.cpp
        test/test_util/UdpChannel.h
        test/test_zookeeper.cpp
        src/msgrpc/core/rpc_sequence_id.cpp include/msgrpc/core/adapter/timer_adapter.h include/msgrpc/core/typedefs.h test/core/adapter/simple_timer_adapter.h test/details/test_constants.h include/msgrpc/core/adapter/config.h include/msgrpc/core/adapter/msg_channel.h test/core/adapter/udp_msg_channel.h test/details/set_timer_handler.h include/msgrpc/core/cell/rsp_sink.h include/msgrpc/core/types.h include/msgrpc/core/components/rsp_dispatcher.h include/msgrpc/core/rpc_result.h include/msgrpc/core/cell/rsp_cell_base.h include/msgrpc/core/cell/cell.h include/msgrpc/core/cell/derived_cell.h include/msgrpc/core/cell/derived_async_cell.h include/msgrpc/core/cell/derived_action.h include/msgrpc/core/cell/timeout_cell.h include/msgrpc/core/macros.h include/msgrpc/core/iface_impl/iface_impl_base.h include/msgrpc/core/iface_impl/iface_repository.h include/msgrpc/core/msg/msg_sender.h include/msgrpc/core/components/req_msg_handler.h include/msgrpc/core/iface_impl/iface_impl_base_t.h include/msgrpc/core/components/rpc_timeout_handler.h include/msgrpc/core/iface_stub/iface_stub_base.h include/msgrpc/core/service_interaction/si_base.h include/msgrpc/core/iface_impl/iface_impl_utility.h test/details/msgrpc_test_loop.h test/details/msgrpc_test.h)

add_executable(msgrpc ${SOURCE_FILES})

target_link_libraries(msgrpc /Users/hmwang/lab/thrift-0.9.3/lib/cpp/.libs/libthrift.a gtest gtest_main zookeeper_mt boost_system)
################################################################################
find_program(VALGRIND "valgrind")
if(VALGRIND)
    add_custom_target(valgrind
            COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes $<TARGET_FILE:msgrpc>)
endif()


#################################################################################
#TODO: splite into more sub porject: http://stackoverflow.com/questions/30239385/is-it-possible-to-configure-clion-to-compile-source-files-in-a-project-independe