cmake_minimum_required(VERSION 2.8)
project(service_x)

################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

################################################################################
LINK_DIRECTORIES(${___GTEST_LIB})
INCLUDE_DIRECTORIES(${___GTEST_INCLUDE})

#using boost to make udp sockets for testcases to simulate IPC
LINK_DIRECTORIES(${___BOOST_LIB})
INCLUDE_DIRECTORIES(${___BOOST_INCLUDE})

################################################################################
LINK_DIRECTORIES(${___ZOOKEEPER_LIB})
INCLUDE_DIRECTORIES(${___ZOOKEEPER_INCLUDE})
add_definitions(-DTHREADED)

################################################################################
include_directories(~/lab/thrift-0.9.3/lib/cpp/src)
include_directories(~/lab/cpp/msgrpc/msgrpc/include)
include_directories(~/lab/cpp/msgrpc/msgrpc)

include_directories(~/lab/cpp/msgrpc/demo_services/service_x/include)

################################################################################
FILE(GLOB_RECURSE SERVICE_X_SOURCE_FILES ./*.cpp ./*.h)

add_executable(service_x ${SERVICE_X_SOURCE_FILES})

target_link_libraries(service_x ${___THRIFT_LIB} gtest zookeeper_mt conservator-framework boost_system msgrpc)

################################################################################
find_program(VALGRIND "valgrind")
if(VALGRIND)
    add_custom_target(valgrind_service_x
            COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes $<TARGET_FILE:service_x>)
endif()
