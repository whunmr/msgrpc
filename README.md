# msgrpc
async RPC implemented by async message-passing

1. using macro to replace thrift IDL, e.g.:
```
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
```

can be write in msgrpc DSL:

```
#define ___fields_of_struct___EmbeddedStruct(_, ...)\
  _(1, es_i8,          int8_t,          __VA_ARGS__)\
  _(2, es_i16,         int16_t,         __VA_ARGS__)

___def_struct(EmbeddedStruct);

typedef std::map<int32_t, std::string> map_int32_string; //constraints: can not contains comma in type field, using typedef to remove comma.
typedef std::map<std::string, EmbeddedStruct> map_string_struct;

#define ___fields_of_struct___ResponseData(_, ...)                            \
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
```