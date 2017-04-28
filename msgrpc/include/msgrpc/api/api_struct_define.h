#ifndef MSGRPC_SERVICE_API_DEFINE_H_H
#define MSGRPC_SERVICE_API_DEFINE_H_H

#include <msgrpc/thrift/thrift_struct_define.h>

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
#ifdef ___def_service
#undef ___def_service
#endif

#define ___def_service(service_name_, version_)      \
namespace service_name_ {                            \
    const char* api_version = #version_;             \
    void msgrpc_register_service() { std::cout << "register service to zk..." << std::endl; }  \
}                                                    \
namespace service_name_

////////////////////////////////////////////////////////////////////////////////
//during struct_declaration, do_nothing for interface related macros.
#ifdef ___as_interface
#undef ___as_interface
#endif

#define ___as_interface(...)
////////////////////////////////////////////////////////////////////////////////

#endif //MSGRPC_SERVICE_API_DEFINE_H_H
