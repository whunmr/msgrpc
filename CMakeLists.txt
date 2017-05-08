cmake_minimum_required(VERSION 2.8)

SET(___THRIFT_LIB ${PROJECT_SOURCE_DIR}/../../../lab/thrift-0.9.3/lib/cpp/.libs/libthrift.a)

SET(___GTEST_INCLUDE ~/lab/repo/googletest/googletest/include)
SET(___GTEST_LIB ~/lab/repo/googletest/googlemock/gtest)

SET(___BOOST_INCLUDE /usr/local/Cellar/boost/1.60.0_1/include)
SET(___BOOST_LIB /usr/local/Cellar/boost/1.60.0_1/lib)


add_subdirectory(msgrpc)
add_subdirectory(demo_services/service_x)
add_subdirectory(demo_services/service_y)
add_subdirectory(demo_services/service_z)
