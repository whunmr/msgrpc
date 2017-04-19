cmake_minimum_required(VERSION 3.3)
project(msgrpc_demo)

################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

################################################################################
#separate libraries depended by msgrpc and testcases
LINK_DIRECTORIES(~/lab/repo/googletest/googlemock/gtest)
INCLUDE_DIRECTORIES(~/lab/repo/googletest/googletest/include)

################################################################################
#using boost to make udp sockets for testcases to simulate IPC
LINK_DIRECTORIES(/usr/local/Cellar/boost/1.60.0_1/lib)
INCLUDE_DIRECTORIES(/usr/local/Cellar/boost/1.60.0_1/include)

################################################################################
LINK_DIRECTORIES(~/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/lib/)
INCLUDE_DIRECTORIES(~/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/include/)
add_definitions(-DTHREADED)

################################################################################
include_directories(~/lab/thrift-0.9.3/lib/cpp/src)
include_directories(~/lab/cpp/msgrpc/msgrpc/include)
include_directories(~/lab/cpp/msgrpc/msgrpc)
include_directories(~/lab/cpp/msgrpc/)

################################################################################
FILE(GLOB_RECURSE DEMO_SOURCE_FILES         demo/*.cpp demo/*.h demo/*.interface)
FILE(GLOB_RECURSE MSGRPC_SOURCE_FILES       include/*.h src/*.cpp)
FILE(GLOB_RECURSE DEMO_TEST_SOURCE_FILES    test/*.cpp test/*.h)

################################################################################
add_library(msgrpc SHARED ${MSGRPC_SOURCE_FILES})
set_target_properties(msgrpc PROPERTIES output_name "msgrpc")

################################################################################
add_executable(msgrpc_demo ${DEMO_SOURCE_FILES} ${DEMO_TEST_SOURCE_FILES})

target_link_libraries(msgrpc_demo ${___THRIFT_LIB} gtest gtest_main zookeeper_mt boost_system msgrpc)

################################################################################
find_program(VALGRIND "valgrind")
if(VALGRIND)
    add_custom_target(valgrind
            COMMAND "${VALGRIND}" --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes --track-origins=yes $<TARGET_FILE:msgrpc_demo>)
endif()
