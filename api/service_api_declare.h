#ifndef MSGRPC_SERVICE_API_DECLARE_H_H
#define MSGRPC_SERVICE_API_DECLARE_H_H

#include <thrift_struct/thrift_struct_declare.h>

////////////////////////////////////////////////////////////////////////////////
#ifdef ___api_version
#undef ___api_version
#endif

#define ___api_version(version)  extern const char* api_version;
////////////////////////////////////////////////////////////////////////////////

#endif //MSGRPC_SERVICE_API_DECLARE_H_H
