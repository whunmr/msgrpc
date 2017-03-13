#include "demo_api_declare.h"

#include "thrift_struct/thrift_struct_define.h"
#include "demo_service.interface"

namespace demo {
    bool EmbeddedStruct::operator<(const EmbeddedStruct &rhs) const {
        return es_i8 < rhs.es_i8 ? true : (es_i8 > rhs.es_i8 ? false : (es_i16 < rhs.es_i16));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
#include "gen1/api_types.h"
namespace org { namespace example { namespace msgrpc { namespace thrift {
    bool EmbeddedStruct::operator<(const EmbeddedStruct &rhs) const {
        return es_i8 < rhs.es_i8 ? true : (es_i8 > rhs.es_i8 ? false : (es_i16 < rhs.es_i16));
    }
}}}}
