
    #define ___fields_of_struct___EmbeddedStruct(_, ...)\
        _(1, es_i8,          int8_t,        __VA_ARGS__)\
        _(2, es_i16,         int16_t,       __VA_ARGS__)

    ___def_struct(EmbeddedStruct);


    /*constraints: can not contains comma in type field, using typedef to remove comma.*/
    typedef std::map<int32_t, std::string> map_int32_string;
    typedef std::map<std::string, EmbeddedStruct> map_string_struct;

    #define ___fields_of_struct___ResponseData(_, ...)                              \
        _(1, pet_id,                 int32_t,                           __VA_ARGS__)\
        _(2, pet_name,               std::string,                       __VA_ARGS__)\
        _(3, pet_weight,             int32_t,                           __VA_ARGS__)\
        _(4, pet_i8_value,           int8_t,                            __VA_ARGS__)\
        _(5, pet_i16_value,          int16_t,                           __VA_ARGS__)\
        _(6, pet_i64_value,          int64_t,                           __VA_ARGS__)\
        _(7, pet_double_value,       double,                            __VA_ARGS__)\
        _(8, pet_bool_value,         bool,                              __VA_ARGS__)\
        _(9, pet_binary_value,       binary,                            __VA_ARGS__)\
        _(10, pet_embedded_struct,   EmbeddedStruct,                    __VA_ARGS__)\
        _(11, pet_list_i32,          std::vector<int32_t>,              __VA_ARGS__)\
        _(12, pet_list_of_struct,    std::vector<EmbeddedStruct>,       __VA_ARGS__)\
        _(13, pet_list_of_bool,      std::vector<bool>,                 __VA_ARGS__)\
        _(14, pet_set_of_i32,        std::set<int32_t>,                 __VA_ARGS__)\
        _(15, pet_set_of_struct,     std::set<EmbeddedStruct>,          __VA_ARGS__)\
        _(16, pet_map_i32_string,    map_int32_string,                  __VA_ARGS__)\
        _(17, pet_map_string_struct, map_string_struct,                 __VA_ARGS__)

    ___def_struct(ResponseData);

