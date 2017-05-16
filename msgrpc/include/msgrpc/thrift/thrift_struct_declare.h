//#ifndef MSGRPC_THRIFT_STRUCT_DEF_IDL_H
//#define MSGRPC_THRIFT_STRUCT_DEF_IDL_H

#include "thrift_struct_common.h"
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
#define ___expand_isset_init_list__required(fid_, opt_or_req_, ftype_, fname_, ...)

#define ___expand_isset_init_list__optional(fid_, opt_or_req_, ftype_, fname_, ...)  , fname_(false)

#define ___expand_isset_init_list(fid_, opt_or_req_, ftype_, fname_, ...) \
   ___expand_isset_init_list__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___expand_isset_field__required(fid_, opt_or_req_, ftype_, fname_, ...)
#define ___expand_isset_field__optional(fid_, opt_or_req_, ftype_, fname_, ...)  bool fname_ :1;
#define ___expand_isset_field(fid_, opt_or_req_, ftype_, fname_, ...) \
    ___expand_isset_field__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___declare_struct_isset(struct_name_) \
    typedef struct _##struct_name_##__isset : DummyParent<_##struct_name_##__isset> { \
        _##struct_name_##__isset() : DummyParent<_##struct_name_##__isset>()\
             ___apply_expand(struct_name_, ___expand_isset_init_list, struct_name_) {} \
        ___apply_expand(struct_name_, ___expand_isset_field, struct_name_) \
    } _##struct_name_##__isset;

////////////////////////////////////////////////////////////////////////////////
#define ___expand_struct_init_list(fid_, opt_or_req_, ftype_, fname_, ...)    ,fname_(ftype_())


#define ___expand_struct_field__required(fid_, opt_or_req_, ftype_, fname_, ...)   public: ftype_ fname_;
#define ___expand_struct_field__optional(fid_, opt_or_req_, ftype_, fname_, ...)   private: ftype_ fname_;
#define ___expand_struct_field(fid_, opt_or_req_, ftype_, fname_, ...) \
              ___expand_struct_field__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)


#define ___expand_get_struct_field_method__required(fid_, opt_or_req_, ftype_, fname_, ...)
#define ___expand_get_struct_field_method__optional(fid_, opt_or_req_, ftype_, fname_, ...) ftype_ get_##fname_() const { assert(__isset.fname_ && "can only access optional field which has value."); return this->fname_; }

#define ___expand_get_struct_field_method(fid_, opt_or_req_, ftype_, fname_, ...) \
              ___expand_get_struct_field_method__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

#define ___expand_declare_set_field_method(fid_, opt_or_req_, ftype_, fname_, ...) \
  void __set_##fname_(const ftype_& val);

////////////////////////////////////////////////////////////////////////////////
#define ___expand_field_compare_statement__required(fid_, opt_or_req_, ftype_, fname_, ...) \
  if (!(fname_ == rhs.fname_)) return false;

#define ___expand_field_compare_statement__optional(fid_, opt_or_req_, ftype_, fname_, ...) \
  if (__isset.fname_ != rhs.__isset.fname_)\
    return false;\
  else if (__isset.fname_ && !(fname_ == rhs.fname_))\
    return false;

#define ___expand_field_compare_statement(fid_, opt_or_req_, ftype_, fname_, ...) \
    ___expand_field_compare_statement__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___declare_struct_self(struct_name_) \
  class struct_name_ : ThriftStruct {\
    public:\
    \
    struct_name_(const struct_name_&);\
    struct_name_& operator=(const struct_name_&);\
    struct_name_() : ThriftStruct()\
      ___apply_expand(struct_name_, ___expand_struct_init_list, struct_name_) {} \
    virtual ~struct_name_() throw();\
    ___apply_expand(struct_name_, ___expand_struct_field, struct_name_)\
    public: \
    ___apply_expand(struct_name_, ___expand_get_struct_field_method, struct_name_)\
    \
    public: \
    _##struct_name_##__isset __isset;\
    \
    ___apply_expand(struct_name_, ___expand_declare_set_field_method, struct_name_)\
    bool operator == (const struct_name_ & rhs) const {\
      ___apply_expand(struct_name_, ___expand_field_compare_statement, struct_name_)\
      return true;\
    }\
    \
    bool operator != (const struct_name_ &rhs) const { return !(*this == rhs); } \
    bool operator < (const struct_name_ & ) const; \
    \
    uint32_t read(::apache::thrift::protocol::TProtocol* iprot);\
    uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;\
    \
    virtual void printTo(std::ostream& out) const;\
    friend void swap(struct_name_ &a, struct_name_ &b);\
  };\
  inline std::ostream& operator<<(std::ostream& out, const struct_name_& obj)\
  {\
    obj.printTo(out);\
    return out;\
  }


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define ___expand_read_field_case__required(fid_, opt_or_req_, ftype_, fname_, ...) \
  case fid_:\
    if (ftype == TTypeT<ftype_>::value) {\
      xfer +=  TTypeT<ftype_>::read(iprot, this->fname_);\
      isset_##fname_ = true;\
    } else {\
      xfer += iprot->skip(ftype);\
    }\
    break;

#define ___expand_read_field_case__optional(fid_, opt_or_req_, ftype_, fname_, ...) \
  case fid_:\
    if (ftype == TTypeT<ftype_>::value) {\
      xfer +=  TTypeT<ftype_>::read(iprot, this->fname_);\
      this->__isset.fname_ = true;\
    } else {\
      xfer += iprot->skip(ftype);\
    }\
    break;

#define ___expand_read_field_case(fid_, opt_or_req_, ftype_, fname_, ...) \
            ___expand_read_field_case__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___expand_read_declare_required_field_readed__required(fid_, opt_or_req_, ftype_, fname_, comment, struct_name_)\
    bool isset_##fname_ = false;

#define ___expand_read_declare_required_field_readed__optional(...)

#define ___expand_read_declare_required_field_readed(fid_, opt_or_req_, ftype_, fname_, ...) \
            ___expand_read_declare_required_field_readed__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___expand_read_check_required_field_received__required(fid_, opt_or_req_, ftype_, fname_, ...)\
          if (!isset_##fname_) throw TProtocolException(TProtocolException::INVALID_DATA);

#define ___expand_read_check_required_field_received__optional(fid_, opt_or_req_, ftype_, fname_, ...)

#define ___expand_read_check_required_field_received(fid_, opt_or_req_, ftype_, fname_, ...)\
            ___expand_read_check_required_field_received__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___expand_write_field__required(fid_, opt_or_req_, ftype_, fname_, ...) \
  xfer += oprot->writeFieldBegin(#fname_, (::apache::thrift::protocol::TType)TTypeT<ftype_>::value, fid_);\
  xfer += TTypeT<ftype_>::write(oprot, this->fname_); \
  xfer += oprot->writeFieldEnd();

#define ___expand_write_field__optional(fid_, opt_or_req_, ftype_, fname_, ...) \
  if (this->__isset.fname_) {\
    xfer += oprot->writeFieldBegin(#fname_, (::apache::thrift::protocol::TType)TTypeT<ftype_>::value, fid_);\
    xfer += TTypeT<ftype_>::write(oprot, this->fname_); \
    xfer += oprot->writeFieldEnd();\
  }

#define ___expand_write_field(fid_, opt_or_req_, ftype_, fname_, ...) \
    ___expand_write_field__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___declare_struct(struct_name_)   \
    ___declare_struct_isset(struct_name_) \
    ___declare_struct_self(struct_name_)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef ___as_struct
#undef ___as_struct
#endif

#define ___as_struct(struct_name_) \
        ___declare_struct(struct_name_)


//#endif //MSGRPC_THRIFT_STRUCT_DEF_IDL_H
