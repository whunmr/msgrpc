namespace cpp org.example.msgrpc.thrift


struct ResponseData {
        1: i32    pet_id,
        2: string pet_name,
        3: i32    pet_weight,
}

/*
struct RequestData {
        1: i32 pet_id,
}

service Petstore {
        ResponseData get_pet_info(1: RequestData req),
}
*/

