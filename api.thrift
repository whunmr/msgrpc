namespace cpp org.example.msgrpc.thrift


struct ResponseData {
        1: i32    pet_id,
        2: string pet_name,
        3: i32    pet_weight,
        4: byte   pet_i8_value,
        5: i16    pet_i16_value,
        6: i64    pet_i64_value,
        7: double pet_double_value,
        8: bool   pet_bool_value
}

/*
struct RequestData {
        1: i32 pet_id,
}

service Petstore {
        ResponseData get_pet_info(1: RequestData req),
}
*/
