cmake_minimum_required(VERSION 3.3)

SET(___THRIFT_LIB ${PROJECT_SOURCE_DIR}/../../../lab/thrift-0.9.3/lib/cpp/.libs/libthrift.a)

add_subdirectory(msgrpc)
add_subdirectory(demo_services/service_x)
add_subdirectory(demo_services/service_y)
add_subdirectory(demo_services/service_z)
