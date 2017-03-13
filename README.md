# msgrpc
async RPC implemented by async message-passing

1. using macro to replace thrift IDL, e.g.:
```
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
```

can be write in msgrpc DSL:

```
namespace demo {
    ___api_version(1.0.3)

    ////////////////////////////////////////////////////////////////////////////
    #define ___fields_of_struct___SingleOptionalFieldStruct(_, ...) \
        _(1, optional, int16_t, value,                  __VA_ARGS__)\
        _(2, required, int64_t, value64,                __VA_ARGS__)
    ___as_struct(SingleOptionalFieldStruct);

    ////////////////////////////////////////////////////////////////////////////
    #define ___fields_of_struct___EmbeddedStruct(_, ...)          \
        _(1, required, int8_t,  es_i8,                __VA_ARGS__)\
        _(2, optional, int16_t, es_i16,               __VA_ARGS__)
    ___as_struct(EmbeddedStruct);

    ////////////////////////////////////////////////////////////////////////////
    typedef std::map <int32_t, std::string> map_int32_string;
    typedef std::map <std::string, EmbeddedStruct> map_string_struct;

    #define ___fields_of_struct___ResponseData(_, ...)                                  \
        _(1,  required, int32_t,                     pet_id,                __VA_ARGS__)\
        _(2,  required, std::string,                 pet_name,              __VA_ARGS__)\
        _(3,  required, int32_t,                     pet_weight,            __VA_ARGS__)\
        _(4,  required, int8_t,                      pet_i8_value,          __VA_ARGS__)\
        _(5,  required, int16_t,                     pet_i16_value,         __VA_ARGS__)\
        _(6,  required, int64_t,                     pet_i64_value,         __VA_ARGS__)\
        _(7,  required, double,                      pet_double_value,      __VA_ARGS__)\
        _(8,  required, bool,                        pet_bool_value,        __VA_ARGS__)\
        _(9,  required, binary,                      pet_binary_value,      __VA_ARGS__)\
        _(10, required, EmbeddedStruct,              pet_embedded_struct,   __VA_ARGS__)\
        _(11, required, std::vector<int32_t>,        pet_list_i32,          __VA_ARGS__)\
        _(12, required, std::vector<EmbeddedStruct>, pet_list_of_struct,    __VA_ARGS__)\
        _(13, optional, std::vector<bool>,           pet_list_of_bool,      __VA_ARGS__)\
        _(14, optional, std::set<int32_t>,           pet_set_of_i32,        __VA_ARGS__)\
        _(15, optional, std::set<EmbeddedStruct>,    pet_set_of_struct,     __VA_ARGS__)\
        _(16, optional, map_int32_string,            pet_map_i32_string,    __VA_ARGS__)\
        _(17, optional, map_string_struct,           pet_map_string_struct, __VA_ARGS__)
    ___as_struct(ResponseData);
}

```