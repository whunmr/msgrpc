cmake_minimum_required(VERSION 2.8)
project(service_z)

################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

################################################################################
LINK_DIRECTORIES   (${___GTEST_LIB})
INCLUDE_DIRECTORIES(${___GTEST_INCLUDE})

#using boost to make udp sockets for testcases to simulate IPC
LINK_DIRECTORIES   (${___BOOST_LIB})
INCLUDE_DIRECTORIES(${___BOOST_INCLUDE})

################################################################################
LINK_DIRECTORIES   (${___ZOOKEEPER_LIB})
INCLUDE_DIRECTORIES(${___ZOOKEEPER_INCLUDE})
add_definitions    (-DTHREADED)

################################################################################
include_directories(${___THRIFT_INCLUDE})
include_directories(${___MSGRPC_INCLUDE})

include_directories(./include)

################################################################################
FILE(GLOB_RECURSE SERVICE_Z_SOURCE_FILES ./*.cpp ./*.h)

add_executable(service_z ${SERVICE_Z_SOURCE_FILES})

target_link_libraries(service_z ${___THRIFT_LIB} gtest zookeeper_mt conservator-framework boost_system msgrpc msgrpc_adapter_example)

################################################################################
find_program(VALGRIND "valgrind")
if(VALGRIND)
    add_custom_target(valgrind_service_z
            COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes $<TARGET_FILE:service_z>)
endif()
