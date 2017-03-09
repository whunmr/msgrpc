#ifndef MSGRPC_THRIFT_STRUCT_DEF_IDL_H
#define MSGRPC_THRIFT_STRUCT_DEF_IDL_H

#include <map>
#include <vector>
#include <string>

#include <thrift/protocol/TProtocol.h>
#include <thrift/TToString.h>

//using namespace ::apache::thrift::protocol;

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
#define ___expand_isset_init_list(fid_, fname_, ftype_, ...)       , fname_(false)
#define ___expand_isset_field(fid_, fname_, ftype_, ...)           bool fname_ :1;

#define ___declare_struct_isset(struct_name_) \
    typedef struct _##struct_name_##__isset : DummyParent<_##struct_name_##__isset> { \
        _##struct_name_##__isset() : DummyParent<_##struct_name_##__isset>()\
             ___apply_expand(struct_name_, ___expand_isset_init_list, struct_name_) {} \
        ___apply_expand(struct_name_, ___expand_isset_field, struct_name_) \
    } _##struct_name_##__isset;

////////////////////////////////////////////////////////////////////////////////
#define ___expand_struct_init_list(fid_, fname_, ftype_, ...)    ,fname_(ftype_())
#define ___expand_struct_field(fid_, fname_, ftype_, ...)        ftype_ fname_;

#define ___expand_declare_set_field_method(fid_, fname_, ftype_, ...) \
  void __set_##fname_(const ftype_& val);

#define ___expand_field_compare_statement(fid_, fname_, ftype_, ...) \
  if (!(fname_ == rhs.fname_)) return false;

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
  };\
  void swap(struct_name_ &a, struct_name_ &b);\
  inline std::ostream& operator<<(std::ostream& out, const struct_name_& obj)\
  {\
    obj.printTo(out);\
    return out;\
  }


////////////////////////////////////////////////////////////////////////////////
#define ___expand_define_set_field_method(fid_, fname_, ftype_, struct_name_) \
    void struct_name_::__set_##fname_(const ftype_& val) {\
      this->fname_ = val;\
    }


////////////////////////////////////////////////////////////////////////////////
/*enum TType {
    T_VOID       = 1,
    T_BOOL       = 2,  -
    T_BYTE       = 3,  -
    T_I08        = 3,  -
    T_I16        = 6,  -
    T_I32        = 8,  -
    T_U64        = 9,
    T_I64        = 10, -
    T_DOUBLE     = 4,  -
    T_STRING     = 11, -
    T_UTF7       = 11,
    T_STRUCT     = 12, -
    T_MAP        = 13, -
    T_SET        = 14, -
    T_LIST       = 15, -
    T_UTF8       = 16,
    T_UTF16      = 17
};*/

struct binary : std::string {};

////////////////////////////////////////////////////////////////////////////////
template<typename T> struct TTypeT {
    enum {value = ::apache::thrift::protocol::T_STRUCT};
    static uint32_t  read(::apache::thrift::protocol::TProtocol* iprot, T& t) {
        return t.read(iprot);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const T& t) {
        return t.write(oprot);
    }
};

template<> struct TTypeT<bool> {
    enum {value = ::apache::thrift::protocol::T_BOOL};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, bool& t) {
        return iprot->readBool(t);
    }
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::vector<bool>::reference t) {
        return iprot->readBool(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const bool& t) {
        return oprot->writeBool(t);
    }
};

template<> struct TTypeT<int8_t> {
    enum {value = ::apache::thrift::protocol::T_BYTE};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, int8_t& t) {
        return iprot->readByte(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const int8_t& t) {
        return oprot->writeByte(t);
    }
};

template<> struct TTypeT<int16_t> {
    enum {value = ::apache::thrift::protocol::T_I16};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, int16_t& t) {
        return iprot->readI16(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const int16_t& t) {
        return oprot->writeI16(t);
    }
};

template<> struct TTypeT<int32_t> {
    enum {value = ::apache::thrift::protocol::T_I32};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, int32_t& t) {
        return iprot->readI32(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const int32_t& t) {
        return oprot->writeI32(t);
    }
};

template<> struct TTypeT<int64_t> {
    enum {value = ::apache::thrift::protocol::T_I64};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, int64_t& t) {
        return iprot->readI64(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const int64_t& t) {
        return oprot->writeI64(t);
    }
};

template<> struct TTypeT<double> {
    enum {value = ::apache::thrift::protocol::T_DOUBLE};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, double& t) {
        return iprot->readDouble(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const double& t) {
        return oprot->writeDouble(t);
    }
};

template<> struct TTypeT<std::string> {
    enum {value = ::apache::thrift::protocol::T_STRING};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::string& t) {
        return iprot->readString(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const std::string& t) {
        return oprot->writeString(t);
    }
};

template<> struct TTypeT<binary> {
    enum {value = ::apache::thrift::protocol::T_STRING};
    static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, binary& t) {
        return iprot->readBinary(t);
    }
    static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const binary& t) {
        return oprot->writeString(t);
    }
};

template<typename T>
struct TTypeT<std::vector<T> > {
enum {value = ::apache::thrift::protocol::T_LIST};
static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::vector<T>& ___t) {
    uint32_t ret = 0;
    ___t.clear();
    uint32_t _size;
    ::apache::thrift::protocol::TType _etype;
    ret += iprot->readListBegin(_etype, _size);

    ___t.resize(_size);
    uint32_t _i;
    for (_i = 0; _i < _size; ++_i)
    {
        ret += TTypeT<T>::read(iprot, ___t[_i]);
    }
    ret += iprot->readListEnd();
    return ret;
}

static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const std::vector<T>& ___t) {
    uint32_t ret = 0;
    ret += oprot->writeListBegin((::apache::thrift::protocol::TType)TTypeT<T>::value, static_cast<uint32_t>(___t.size()));

    typename std::vector<T>::const_iterator _iter;
    for (_iter = ___t.begin(); _iter != ___t.end(); ++_iter)
    {
        ret += TTypeT<T>::write(oprot, (*_iter));
    }

    ret += oprot->writeListEnd();
    return ret;
}
};

template<typename T>
struct TTypeT<std::set<T> > {
enum {value = ::apache::thrift::protocol::T_SET};
static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::set<T>& ___t) {
    uint32_t ret = 0;
    uint32_t _size;
    ::apache::thrift::protocol::TType _etype;
    ret += iprot->readSetBegin(_etype, _size);

    uint32_t _i;
    for (_i = 0; _i < _size; ++_i)
    {
        T _elem;
        ret += TTypeT<T>::read(iprot, _elem);
        ___t.insert(_elem);
    }
    ret += iprot->readSetEnd();

    return ret;
}

static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const std::set<T>& ___t) {
    uint32_t ret = 0;

    ret += oprot->writeSetBegin((::apache::thrift::protocol::TType)TTypeT<T>::value, static_cast<uint32_t>(___t.size()));
    typename std::set<T>::const_iterator _iter;
    for (_iter = ___t.begin(); _iter != ___t.end(); ++_iter)
    {
        ret += TTypeT<T>::write(oprot, (*_iter));
    }
    ret += oprot->writeSetEnd();

    return ret;
}
};

template<typename _K, typename _V>
struct TTypeT<std::map<_K, _V> > {
enum {value = ::apache::thrift::protocol::T_MAP};
static uint32_t read(::apache::thrift::protocol::TProtocol* iprot, std::map<_K, _V>& ___t) {
    uint32_t ret = 0;

    ___t.clear();
    uint32_t _size;
    ::apache::thrift::protocol::TType _ktype;
    ::apache::thrift::protocol::TType _vtype;
    ret += iprot->readMapBegin(_ktype, _vtype, _size);

    uint32_t _i;
    for (_i = 0; _i < _size; ++_i) {
        _K _key;
        ret += TTypeT<_K>::read(iprot, _key);
        _V &_val = ___t[_key];
        ret += TTypeT<_V>::read(iprot, _val);
    }
    ret += iprot->readMapEnd();

    return ret;
}

static uint32_t write(::apache::thrift::protocol::TProtocol* oprot, const std::map<_K, _V>& ___t) {
    uint32_t ret = 0;

    ret += oprot->writeMapBegin((::apache::thrift::protocol::TType)TTypeT<_K>::value
                                , (::apache::thrift::protocol::TType)TTypeT<_V>::value
                                , static_cast<uint32_t>(___t.size()));
    typename std::map<_K, _V> ::const_iterator _iter;
    for (_iter = ___t.begin(); _iter != ___t.end(); ++_iter)
    {
        ret += TTypeT<_K>::write(oprot, _iter->first);
        ret += TTypeT<_V>::write(oprot, _iter->second);
    }
    ret += oprot->writeMapEnd();

    return ret;
}
};


#define ___expand_read_field_case(fid_, fname_, ftype_, ...) \
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


#define ___expand_write_field(fid_, fname_, ftype_, ...) \
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

#define ___expand_swap_field(fid_, fname_, ftype_, ...) \
    swap(a.fname_, b.fname_);

#define ___define_swap(struct_name_) \
    void swap(struct_name_ &a, struct_name_ &b) {\
        using ::std::swap;\
        ___apply_expand(struct_name_, ___expand_swap_field, struct_name_)\
        swap(a.__isset, b.__isset);\
    }


#define ___expand_assign_field(fid_, fname_, ftype_, ...) \
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

#define ___expand_print_out_field(fid_, fname_, ftype_, ...) \
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
