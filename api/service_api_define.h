#ifndef MSGRPC_SERVICE_API_DEFINE_H_H
#define MSGRPC_SERVICE_API_DEFINE_H_H

#include <thrift_struct/thrift_struct_define.h>

////////////////////////////////////////////////////////////////////////////////
#ifdef ___api_version
#undef ___api_version
#endif
#define ___api_version(version)  const char* api_version = #version;
////////////////////////////////////////////////////////////////////////////////

#endif //MSGRPC_SERVICE_API_DEFINE_H_H
