#include <iostream>
#include "gen1/api_types.h"

using namespace org::example::msgrpc;

#include "thrift/thrift_struct_def_idl.h"
namespace demo {
    #include "demo_service_interface.h"
}

namespace demo {
    bool EmbeddedStruct::operator<(const EmbeddedStruct &rhs) const {
        return es_i8 < rhs.es_i8 ? true : (es_i8 > rhs.es_i8 ? false : (es_i16 < rhs.es_i16));
    }
}

namespace org { namespace example { namespace msgrpc { namespace thrift {
    bool EmbeddedStruct::operator<(const EmbeddedStruct &rhs) const {
        return es_i8 < rhs.es_i8 ? true : (es_i8 > rhs.es_i8 ? false : (es_i16 < rhs.es_i16));
    }
}}}}

////////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "thrift/thrift_codec.h"

int main() {
    thrift::ResponseData ___foo;
    ___foo.pet_id = 11;
    ___foo.pet_name = "pet_name_foo";
    ___foo.pet_weight = 32;
    ___foo.pet_i8_value = 8;
    ___foo.pet_i16_value = 16;
    ___foo.pet_i64_value = 64;
    ___foo.pet_double_value = 3.3;
    ___foo.pet_bool_value = true;
    ___foo.pet_binary_value = string("abcd");

    ___foo.pet_embedded_struct.es_i8 = 88;
    ___foo.pet_embedded_struct.es_i16 = 1616;

    ___foo.pet_list_i32.push_back(9);
    ___foo.pet_list_i32.push_back(10);

    thrift::EmbeddedStruct es1; es1.es_i8 = 97; es1.es_i16 = 116;
    thrift::EmbeddedStruct es2; es2.es_i8 = 98; es2.es_i16 = 216;

    ___foo.pet_list_of_struct.push_back(es1);
    ___foo.pet_list_of_struct.push_back(es2);

    ___foo.pet_list_of_bool.push_back(true);
    ___foo.pet_list_of_bool.push_back(false);
    ___foo.pet_list_of_bool.push_back(true);

    ___foo.pet_set_of_i32.insert(100);
    ___foo.pet_set_of_i32.insert(101);
    ___foo.pet_set_of_i32.insert(102);

    ___foo.pet_set_of_struct.insert(es1);
    ___foo.pet_set_of_struct.insert(es2);

    ___foo.pet_map_i32_string[200] = "hello";
    ___foo.pet_map_i32_string[201] = "world";

    ___foo.pet_map_string_struct["foo"] = es1;
    ___foo.pet_map_string_struct["bar"] = es2;

    uint8_t* pbuf; uint32_t len;

    if (ThriftEncoder::encode(___foo, &pbuf, &len)) {
        demo::ResponseData ___bar;

        if (ThriftDecoder::decode(___bar, pbuf, len)) {
            cout << ___bar.pet_id << endl;
            cout << ___bar.pet_name << endl;
            cout << ___bar.pet_weight << endl;
            cout << (int)___bar.pet_i8_value << endl;
            cout << ___bar.pet_i16_value << endl;
            cout << ___bar.pet_i64_value << endl;
            cout << ___bar.pet_double_value << endl;
            cout << ___bar.pet_bool_value << endl;
            cout << ___bar.pet_binary_value << endl;
            cout << (int)___bar.pet_embedded_struct.es_i8 << endl;
            cout << ___bar.pet_embedded_struct.es_i16 << endl;
            cout << ___bar.pet_list_i32.at(0) << endl;
            cout << ___bar.pet_list_i32.at(1) << endl;

            cout << (int)___bar.pet_list_of_struct.at(0).es_i8 << endl;
            cout << ___bar.pet_list_of_struct.at(0).es_i16 << endl;

            cout << (int)___bar.pet_list_of_struct.at(1).es_i8 << endl;
            cout << ___bar.pet_list_of_struct.at(1).es_i16 << endl;

            cout << ___bar.pet_list_of_bool.at(0) << endl;
            cout << ___bar.pet_list_of_bool.at(1) << endl;
            cout << ___bar.pet_list_of_bool.at(2) << endl;

            for (auto i : ___bar.pet_set_of_i32) { cout << i << endl; }

            for (auto es : ___bar.pet_set_of_struct) { cout << es << endl; }

            for (auto kv : ___bar.pet_map_i32_string) { cout << kv.first << "=>" << kv.second << endl; }

            if (ThriftEncoder::encode(___bar, &pbuf, &len)) {
                thrift::ResponseData ___foo2;
                if (ThriftDecoder::decode(___foo2, pbuf, len)) {
                    cout << ___foo << endl;
                    cout << ___foo2 << endl;
                    cout << ___bar << endl;

                    cout << "thrift___foo => msgrpc___bar => thrift___foo2, thrift___foo == thrift___foo2 ? "
                         << (___foo == ___foo2) << endl;
                }
            }
        }
    }

    return 0;
}
