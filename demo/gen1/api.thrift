namespace cpp org.example.msgrpc.thrift

struct SingleOptionalFieldStruct {
    1: optional i16  value,
    2: required i64  value64,
}

struct EmbeddedStruct {
    1: required byte es_i8,
    2: optional i16  es_i16,
}

struct ResponseData {
    1:  required i32                         pet_id,
    2:  required string                      pet_name,
    3:  required i32                         pet_weight,
    4:  required byte                        pet_i8_value,
    5:  required i16                         pet_i16_value,
    6:  required i64                         pet_i64_value,
    7:  required double                      pet_double_value,
    8:  required bool                        pet_bool_value,
    9:  required binary                      pet_binary_value,
    10: required EmbeddedStruct              pet_embedded_struct,
    11: required list<i32>                   pet_list_i32,
    12: required list<EmbeddedStruct>        pet_list_of_struct,
    13: optional list<bool>                  pet_list_of_bool,
    14: optional set<i32>                    pet_set_of_i32,
    15: optional set<EmbeddedStruct>         pet_set_of_struct,
    16: optional map<i32, string>            pet_map_i32_string,
    17: optional map<string, EmbeddedStruct> pet_map_string_struct,
}

/*
struct RequestData {
    1: i32 pet_id,
}

service Petstore {
    ResponseData get_pet_info(1: RequestData req),
}
*/
