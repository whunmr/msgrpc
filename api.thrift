namespace cpp org.example.msgrpc.thrift

struct EmbeddedStruct {
        1: byte  es_i8;
        2: i16   es_i16;
}

struct ResponseData {
        1:  i32                  pet_id,
        2:  string               pet_name,
        3:  i32                  pet_weight,
        4:  byte                 pet_i8_value,
        5:  i16                  pet_i16_value,
        6:  i64                  pet_i64_value,
        7:  double               pet_double_value,
        8:  bool                 pet_bool_value,
        9:  binary               pet_binary_value,
        10: EmbeddedStruct       pet_embedded_struct,
        11: list<i32>            pet_list_i32,
        12: list<EmbeddedStruct> pet_list_of_struct,
        13: list<bool>           pet_list_of_bool,
        14: set<i32>             pet_set_of_i32,
        15: set<EmbeddedStruct>  pet_set_of_struct,
        16: map<i32, string>     pet_map_i32_string,
        17: map<string, EmbeddedStruct> pet_map_string_struct,
}

/*
struct RequestData {
        1: i32 pet_id,
}

service Petstore {
        ResponseData get_pet_info(1: RequestData req),
}
*/
