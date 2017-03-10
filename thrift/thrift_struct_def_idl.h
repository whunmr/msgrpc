#ifndef MSGRPC_THRIFT_STRUCT_DEF_IDL_H
#define MSGRPC_THRIFT_STRUCT_DEF_IDL_H

#include <map>
#include <vector>
#include <string>

#include <thrift/protocol/TProtocol.h>
#include <thrift/TToString.h>

#include "thrift_types.h"

////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct DummyParent {};

struct ThriftStruct {};

////////////////////////////////////////////////////////////////////////////////
#define ___apply_expand(struct_name_, field_expand_macro_, ...) \
    ___fields_of_struct___##struct_name_(field_expand_macro_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
//typedef struct _ResponseData__isset {
//    _ResponseData__isset() : pet_id(false), pet_name(false), pet_weight(false) {}
//    bool pet_id :1;
//    bool pet_name :1;
//    bool pet_weight :1;
//} _ResponseData__isset;
#define ___expand_isset_init_list(fid_, opt_or_req_, fname_, ftype_, ...)       , fname_(false)
#define ___expand_isset_field(fid_, opt_or_req_, fname_, ftype_, ...)           bool fname_ :1;

#define ___declare_struct_isset(struct_name_) \
    typedef struct _##struct_name_##__isset : DummyParent<_##struct_name_##__isset> { \
        _##struct_name_##__isset() : DummyParent<_##struct_name_##__isset>()\
             ___apply_expand(struct_name_, ___expand_isset_init_list, struct_name_) {} \
        ___apply_expand(struct_name_, ___expand_isset_field, struct_name_) \
    } _##struct_name_##__isset;

////////////////////////////////////////////////////////////////////////////////
#define ___expand_struct_init_list(fid_, opt_or_req_, fname_, ftype_, ...)    ,fname_(ftype_())


#define ___expand_struct_field__required(fid_, opt_or_req_, fname_, ftype_, ...)   public: ftype_ fname_;
#define ___expand_struct_field__optional(fid_, opt_or_req_, fname_, ftype_, ...)   private: ftype_ fname_;
#define ___expand_struct_field(fid_, opt_or_req_, fname_, ftype_, ...) \
              ___expand_struct_field__##opt_or_req_(fid_, opt_or_req_, fname_, ftype_, __VA_ARGS__)

#define ___expand_declare_set_field_method(fid_, opt_or_req_, fname_, ftype_, ...) \
  void __set_##fname_(const ftype_& val);

#define ___expand_field_compare_statement__required(fid_, opt_or_req_, fname_, ftype_, ...) \
  if (!(fname_ == rhs.fname_)) return false;

#define ___expand_field_compare_statement__optional(fid_, opt_or_req_, fname_, ftype_, ...) \
  if (__isset.fname_ != rhs.__isset.fname_)\
    return false;\
  else if (__isset.fname_ && !(fname_ == rhs.fname_))\
    return false;

#define ___expand_field_compare_statement(fid_, opt_or_req_, fname_, ftype_, ...) \
    ___expand_field_compare_statement__##opt_or_req_(fid_, opt_or_req_, fname_, ftype_, __VA_ARGS__)


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
#define ___expand_define_set_field_method__required(fid_, opt_or_req_, fname_, ftype_, struct_name_) \
    void struct_name_::__set_##fname_(const ftype_& val) {\
      this->fname_ = val;\
    }

#define ___expand_define_set_field_method__optional(fid_, opt_or_req_, fname_, ftype_, struct_name_) \
    void struct_name_::__set_##fname_(const ftype_& val) {\
      this->fname_ = val;\
      __isset.fname_ = true;\
    }

#define ___expand_define_set_field_method(fid_, opt_or_req_, fname_, ftype_, struct_name_) \
    ___expand_define_set_field_method__##opt_or_req_(fid_, opt_or_req_, fname_, ftype_, struct_name_)


#define ___expand_read_field_case(fid_, opt_or_req_, fname_, ftype_, ...) \
  case fid_:\
    if (ftype == TTypeT<ftype_>::value) {\
      xfer +=  TTypeT<ftype_>::read(iprot, this->fname_);\
      this->__isset.fname_ = true;\
    } else {\
      xfer += iprot->skip(ftype);\
    }\
    break;

#define ___define_read(struct_name_) \
    uint32_t struct_name_::read(::apache::thrift::protocol::TProtocol* iprot) {\
      apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);\
      uint32_t xfer = 0;\
      std::string fname;\
      ::apache::thrift::protocol::TType ftype;\
      int16_t fid;\
      \
      xfer += iprot->readStructBegin(fname);\
      \
      using ::apache::thrift::protocol::TProtocolException;\
      \
      \
      while (true)\
      {\
        xfer += iprot->readFieldBegin(fname, ftype, fid);\
        if (ftype == ::apache::thrift::protocol::T_STOP) {\
          break;\
        }\
        switch (fid)\
        {\
          ___apply_expand(struct_name_, ___expand_read_field_case, struct_name_) \
          default:\
            xfer += iprot->skip(ftype);\
            break;\
        }\
        xfer += iprot->readFieldEnd();\
      }\
      \
      xfer += iprot->readStructEnd();\
      \
      return xfer;\
    }


#define ___expand_write_field(fid_, opt_or_req_, fname_, ftype_, ...) \
    xfer += oprot->writeFieldBegin(#fname_, (::apache::thrift::protocol::TType)TTypeT<ftype_>::value, fid_);\
    xfer += TTypeT<ftype_>::write(oprot, this->fname_); \
    xfer += oprot->writeFieldEnd();

#define ___define_write(struct_name_) \
    uint32_t struct_name_::write(::apache::thrift::protocol::TProtocol* oprot) const {\
      uint32_t xfer = 0;\
      apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);\
      xfer += oprot->writeStructBegin(#struct_name_);\
      \
      ___apply_expand(struct_name_, ___expand_write_field, struct_name_) \
      \
      xfer += oprot->writeFieldStop();\
      xfer += oprot->writeStructEnd();\
      return xfer;\
    }

#define ___expand_swap_field(fid_, opt_or_req_, fname_, ftype_, ...) \
    swap(a.fname_, b.fname_);

#define ___define_swap(struct_name_) \
    void swap(struct_name_ &a, struct_name_ &b) {\
        using ::std::swap;\
        ___apply_expand(struct_name_, ___expand_swap_field, struct_name_)\
        swap(a.__isset, b.__isset);\
    }

#define ___expand_assign_field(fid_, opt_or_req_, fname_, ftype_, ...) \
    fname_ = other0.fname_;

#define ___define_copy_constructor(struct_name_) \
    struct_name_::struct_name_(const struct_name_& other0) {\
      ___apply_expand(struct_name_, ___expand_assign_field, struct_name_)\
      __isset = other0.__isset;\
    }

#define ___define_assign_constructor(struct_name_) \
    struct_name_& struct_name_::operator=(const struct_name_& other0) {\
        ___apply_expand(struct_name_, ___expand_assign_field, struct_name_)\
        __isset = other0.__isset;\
        return *this;\
    }

#define ___expand_print_out_field(fid_, opt_or_req_, fname_, ftype_, ...) \
    out << ", "#fname_"=" << to_string(fname_);

#define ___define_print_to(struct_name_) \
    void struct_name_::printTo(std::ostream& out) const {\
        using ::apache::thrift::to_string;\
        out << #struct_name_"(";\
        ___apply_expand(struct_name_, ___expand_print_out_field, struct_name_)\
        out << ")";\
    }\

#define ___define_struct_self(struct_name_) \
    struct_name_::~struct_name_() throw() {} \
    ___apply_expand(struct_name_, ___expand_define_set_field_method, struct_name_)\
    ___define_read(struct_name_)\
    ___define_write(struct_name_)\
    ___define_swap(struct_name_)\
    ___define_copy_constructor(struct_name_)\
    ___define_assign_constructor(struct_name_)\
    ___define_print_to(struct_name_)

////////////////////////////////////////////////////////////////////////////////
#define ___declare_struct(struct_name_)   \
    ___declare_struct_isset(struct_name_) \
    ___declare_struct_self(struct_name_)

#define ___define_struct(struct_name_)  \
    ___define_struct_self(struct_name_)

#define ___def_struct(struct_name_) \
    ___declare_struct(struct_name_) \
    ___define_struct(struct_name_)

////////////////////////////////////////////////////////////////////////////////

#endif //MSGRPC_THRIFT_STRUCT_DEF_IDL_H
