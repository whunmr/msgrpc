#ifndef MSGRPC_THRIFT_IDL_DEFINE_H
#define MSGRPC_THRIFT_IDL_DEFINE_H

#include "thrift_struct_common.h"

////////////////////////////////////////////////////////////////////////////////
#define ___expand_define_set_field_method__required(fid_, opt_or_req_, ftype_, fname_, comment, struct_name_) \
    void struct_name_::__set_##fname_(const ftype_& val) {\
      this->fname_ = val;\
    }

#define ___expand_define_set_field_method__optional(fid_, opt_or_req_, ftype_, fname_, comment, struct_name_) \
    void struct_name_::__set_##fname_(const ftype_& val) {\
      this->fname_ = val;\
      __isset.fname_ = true;\
    }

#define ___expand_define_set_field_method(fid_, opt_or_req_, ftype_, fname_, comment, struct_name_) \
    ___expand_define_set_field_method__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, comment, struct_name_)

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
      ___apply_expand(struct_name_, ___expand_read_declare_required_field_readed, struct_name_)\
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
      ___apply_expand(struct_name_, ___expand_read_check_required_field_received, struct_name_)\
      return xfer;\
    }


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

////////////////////////////////////////////////////////////////////////////////
#define ___expand_swap_field(fid_, opt_or_req_, ftype_, fname_, ...) \
    swap(a.fname_, b.fname_);

#define ___define_swap(struct_name_) \
    void swap(struct_name_ &a, struct_name_ &b) {\
        using ::std::swap;\
        ___apply_expand(struct_name_, ___expand_swap_field, struct_name_)\
        swap(a.__isset, b.__isset);\
    }

////////////////////////////////////////////////////////////////////////////////
#define ___expand_assign_field(fid_, opt_or_req_, ftype_, fname_, ...) \
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

////////////////////////////////////////////////////////////////////////////////
#define ___expand_print_out_field__required(fid_, opt_or_req_, ftype_, fname_, ...) \
    out << ", "#fname_"=" << to_string(fname_);

#define ___expand_print_out_field__optional(fid_, opt_or_req_, ftype_, fname_, ...) \
    out << ", "#fname_"="; (__isset.fname_ ? (out << to_string(fname_)) : (out << "<null>"));

#define ___expand_print_out_field(fid_, opt_or_req_, ftype_, fname_, ...) \
    ___expand_print_out_field__##opt_or_req_(fid_, opt_or_req_, ftype_, fname_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___define_print_to(struct_name_) \
    void struct_name_::printTo(std::ostream& out) const {\
        using ::apache::thrift::to_string;\
        out << #struct_name_"(";\
        ___apply_expand(struct_name_, ___expand_print_out_field, struct_name_)\
        out << ")";\
    }\

////////////////////////////////////////////////////////////////////////////////
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
#ifdef ___as_struct
#undef ___as_struct
#endif

#define ___as_struct(struct_name_) \
    ___define_struct_self(struct_name_)


#endif //MSGRPC_THRIFT_IDL_DEFINE_H
