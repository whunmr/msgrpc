//#ifndef MSGRPC_SERVICE_API_DECLARE_H_H
//#define MSGRPC_SERVICE_API_DECLARE_H_H

#include <msgrpc/thrift/thrift_struct_declare.h>

////////////////////////////////////////////////////////////////////////////////
#ifdef ___def_service
#undef ___def_service
#endif

#define ___def_service(service_name_, version_)         \
namespace service_name_ {                               \
    extern const char* g_service_name;                  \
    extern const char* g_api_version;                   \
    void msgrpc_register_service(const char* endpoint); \
}                                                       \
namespace service_name_

//#endif //MSGRPC_SERVICE_API_DECLARE_H_H

////////////////////////////////////////////////////////////////////////////////
//during struct_declaration, do_nothing for interface related macros.
#ifdef ___as_interface
#undef ___as_interface
#endif

#define ___as_interface(...)
////////////////////////////////////////////////////////////////////////////////
