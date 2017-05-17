cmake_minimum_required(VERSION 2.8)

SET(___THRIFT_LIB        ${PROJECT_SOURCE_DIR}/../../../lab/thrift-0.9.3/lib/cpp/.libs/libthrift.a)

SET(___GTEST_INCLUDE     ~/lab/repo/googletest/googletest/include)
SET(___GTEST_LIB         ~/lab/repo/googletest/googlemock/gtest)

SET(___BOOST_INCLUDE     /usr/local/Cellar/boost/1.60.0_1/include)
SET(___BOOST_LIB         /usr/local/Cellar/boost/1.60.0_1/lib)

SET(___ZOOKEEPER_LIB     ~/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/lib/)
SET(___ZOOKEEPER_INCLUDE ~/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/include/
                         ~/lab/zookeeper/zookeeper-3.4.9/build/c/build/usr/include/zookeeper)


SET(___THRIFT_INCLUDE    ~/lab/thrift-0.9.3/lib/cpp/src)

SET(___MSGRPC_INCLUDE    ~/lab/cpp/msgrpc/msgrpc/include
                         ~/lab/cpp/msgrpc/msgrpc)


add_subdirectory(msgrpc)
add_subdirectory(demo_services/service_x)
add_subdirectory(demo_services/service_y)
add_subdirectory(demo_services/service_z)
add_subdirectory(demo_services/service_k)


#TODO: add batch service list support, like erlang_rpc's mcall
#TODO: testing support
#TODO: auto generate testing from SI
#TODO: contract testing
#TODO: test log call flow draw
#TODO: msgrpc message capture and save
#TODO: wireshare plugin support to open captured msgrpc messages
#TODO: how to share thrift struct definitions between services