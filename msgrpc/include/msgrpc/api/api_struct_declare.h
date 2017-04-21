#ifndef MSGRPC_SERVICE_API_DECLARE_H_H
#define MSGRPC_SERVICE_API_DECLARE_H_H

#include <msgrpc/thrift/thrift_struct_declare.h>

////////////////////////////////////////////////////////////////////////////////
#ifdef ___api_version
#undef ___api_version
#endif

#define ___api_version(version)  extern const char* api_version;

////////////////////////////////////////////////////////////////////////////////
//during struct_declaration, do_nothing for interface related macros.
#ifdef ___as_interface
#undef ___as_interface
#endif

#define ___as_interface(...)
////////////////////////////////////////////////////////////////////////////////

#endif //MSGRPC_SERVICE_API_DECLARE_H_H
