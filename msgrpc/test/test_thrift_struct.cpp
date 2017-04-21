#include "demo/gen1/api_types.h"
#include "demo/demo_api_struct_declare.h"

#include <msgrpc/thrift/thrift_codec.h>
#include <gtest/gtest.h>
using namespace std;
using namespace org::example::msgrpc;
using namespace msgrpc;

template<typename T, typename M>
void expect_thrift_encoded_buffer__can_decoded_by_msgrpc_____and_vise_versa(T &___t, M &___m) {
    T ___t2;

    uint8_t* pbuf; uint32_t len;

    EXPECT_TRUE(ThriftEncoder::encode(___t, &pbuf, &len));
    EXPECT_TRUE(ThriftDecoder::decode(___m, pbuf, len));
    EXPECT_TRUE(ThriftEncoder::encode(___m, &pbuf, &len));
    EXPECT_TRUE(ThriftDecoder::decode(___t2, pbuf, len));

    EXPECT_EQ(___t, ___t2);
}

TEST(thrift_struct, should_decoded_failed_if_required_field_are_not_setted) {
    thrift::SingleOptionalFieldStruct ___t;
    ___t.__set_value(100);

    demo::SingleOptionalFieldStruct ___m;

    expect_thrift_encoded_buffer__can_decoded_by_msgrpc_____and_vise_versa(___t, ___m);
};

TEST(thrift_struct, should_decoded_failed_if_required_field_are_not_setted____reversed) {
    demo::SingleOptionalFieldStruct ___t;
    ___t.__set_value(100);

    thrift::SingleOptionalFieldStruct ___m;

    expect_thrift_encoded_buffer__can_decoded_by_msgrpc_____and_vise_versa(___t, ___m);
};

TEST(thrift_struct, test_complex_data_types) {
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

    ___foo.pet_embedded_struct.es_i8 = 99;
    ___foo.pet_embedded_struct.__set_es_i8(99);
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


    demo::ResponseData ___bar;

    expect_thrift_encoded_buffer__can_decoded_by_msgrpc_____and_vise_versa(___foo, ___bar);
}

TEST(thrift_struct, test_should_able_to__encode_and_decode___large_object) {
    thrift::ResponseData ___foo;

    thrift::EmbeddedStruct es1; es1.es_i8 = 97; es1.es_i16 = 116;
    thrift::EmbeddedStruct es2; es2.es_i8 = 98; es2.es_i16 = 216;

    for (int i = 0; i < 1000; ++i) {
        ___foo.pet_list_of_struct.push_back(es1);
        ___foo.pet_list_of_struct.push_back(es2);
    }

    demo::ResponseData ___bar;

    expect_thrift_encoded_buffer__can_decoded_by_msgrpc_____and_vise_versa(___foo, ___bar);
}



