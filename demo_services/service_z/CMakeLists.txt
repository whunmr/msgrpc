cmake_minimum_required(VERSION 2.8)
project(service_z)

################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

################################################################################
LINK_DIRECTORIES(${___GTEST_LIB})
INCLUDE_DIRECTORIES(${___GTEST_INCLUDE})

#using boost to make udp sockets for testcases to simulate IPC
LINK_DIRECTORIES(${___BOOST_LIB})
INCLUDE_DIRECTORIES(${___BOOST_INCLUDE})


LINK_DIRECTORIES(~/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/lib/)
INCLUDE_DIRECTORIES(~/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/include/)
INCLUDE_DIRECTORIES(~/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/include/zookeeper)
add_definitions(-DTHREADED)

################################################################################
include_directories(~/lab/thrift-0.9.3/lib/cpp/src)
include_directories(~/lab/cpp/msgrpc/msgrpc/include)
include_directories(~/lab/cpp/msgrpc/msgrpc)

include_directories(~/lab/cpp/msgrpc/demo_services/service_z/include)

################################################################################
FILE(GLOB_RECURSE SERVICE_Y_SOURCE_FILES ./*.cpp ./*.h)

add_executable(service_z ${SERVICE_Y_SOURCE_FILES})

target_link_libraries(service_z ${___THRIFT_LIB} gtest zookeeper_mt conservator-framework boost_system msgrpc)

################################################################################
find_program(VALGRIND "valgrind")
if(VALGRIND)
    add_custom_target(valgrind_service_z
            COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes $<TARGET_FILE:service_z>)
endif()
