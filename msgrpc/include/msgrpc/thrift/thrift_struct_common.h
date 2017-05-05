//#ifndef MSGRPC_THRIFT_IDL_COMMON_H
//#define MSGRPC_THRIFT_IDL_COMMON_H

#include <map>
#include <vector>
#include <string>

#include <thrift/protocol/TProtocol.h>
#include <thrift/TToString.h>

#include "thrift_types.h"


#ifndef MSGRPC_DUMMY_PARENT_DEF
#define MSGRPC_DUMMY_PARENT_DEF

////////////////////////////////////////////////////////////////////////////////
template<typename T> struct DummyParent {};

struct ThriftStruct {};

#endif //MSGRPC_DUMMY_PARENT_DEF


////////////////////////////////////////////////////////////////////////////////
#ifdef ___apply_expand
#undef ___apply_expand
#endif
#define ___apply_expand(struct_name_, field_expand_macro_, ...) \
    ___fields_of_struct___##struct_name_(field_expand_macro_, __VA_ARGS__)

//#endif //MSGRPC_THRIFT_IDL_COMMON_H
