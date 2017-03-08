#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;


#include "gen1/api_types.h"
using namespace org::example::msgrpc;


////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct Singleton {
    virtual ~Singleton() { }

    static T& instance() {
        static T t;
        return t;
    }
};

struct ThriftCodecBase {
    ThriftCodecBase() : mem_buf_(new TMemoryBuffer())
            , protocol_(new TBinaryProtocol(mem_buf_)) { }
          //, protocol_(new TJSONProtocol(mem_buf_)) { }

protected:
    boost::shared_ptr<TMemoryBuffer>  mem_buf_;
    boost::shared_ptr<TBinaryProtocol> protocol_;
    //boost::shared_ptr<TJSONProtocol> protocol_;
};

struct ThriftEncoder : ThriftCodecBase, Singleton<ThriftEncoder> {
    template<typename T>
    static bool encode(const T& ___struct, uint8_t** buf, uint32_t* len) {
        return ThriftEncoder::instance().do_encode(___struct, buf, len);
    }

private:
    template<typename T>
    bool do_encode(const T& ___struct, uint8_t **buf, uint32_t *len) {
        *len = 0;

        try {
            ___struct.write(protocol_.get());
            mem_buf_->getBuffer(buf, len);
        } catch (...) {
            //TODO: add debug log
            return false;
        }

        return *len != 0;
    }
};


struct ThriftDecoder : ThriftCodecBase, Singleton<ThriftDecoder> {
    template<typename T>
    static bool decode(T& ___struct, uint8_t* buf, uint32_t len) {
        ThriftDecoder::instance().mem_buf_->resetBuffer(buf, len, TMemoryBuffer::MemoryPolicy::OBSERVE);
        return ThriftDecoder::instance().do_decode(___struct);
    }

private:
    template<typename T>
    bool do_decode(T& ___struct) {
        try {
            return ___struct.read(protocol_.get()) > 0;
        } catch (...) {
            //TODO: add debug log
            return false;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
template<typename T>
struct DummyParent {};

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
#define ___expand_isset_init_list(fid_, fname_, ftype_)       , fname_(false)
#define ___expand_isset_field(fid_, fname_, ftype_)           bool fname_ :1;

#define ___declare_struct_isset(struct_name_) \
    typedef struct _##struct_name_##__isset : DummyParent<_##struct_name_##__isset> { \
        _##struct_name_##__isset() : DummyParent<_##struct_name_##__isset>() ___apply_expand(struct_name_, ___expand_isset_init_list) {} \
        ___apply_expand(struct_name_, ___expand_isset_field) \
    } _##struct_name_##__isset;

////////////////////////////////////////////////////////////////////////////////
#define ___expand_struct_init_list(fid_, fname_, ftype_)    ,fname_(ftype_())
#define ___expand_struct_field(fid_, fname_, ftype_)        ftype_ fname_;

#define ___expand_declare_set_field_method(fid_, fname_, ftype_) \
  void __set_##fname_(const ftype_& val);

#define ___expand_field_compare_statement(fid_, fname_, ftype_) \
  if (!(fname_ == rhs.fname_)) return false;

#define ___declare_struct_self(struct_name_) \
  class struct_name_ : DummyParent<struct_name_> {\
    public:\
    \
    struct_name_(const struct_name_&);\
    struct_name_& operator=(const struct_name_&);\
    struct_name_() : DummyParent<struct_name_>() ___apply_expand(struct_name_, ___expand_struct_init_list) {} \
    virtual ~struct_name_() throw();\
    ___apply_expand(struct_name_, ___expand_struct_field)\
    \
    _##struct_name_##__isset __isset;\
    \
    ___apply_expand(struct_name_, ___expand_declare_set_field_method)\
    bool operator == (const struct_name_ & rhs) const {\
      ___apply_expand(struct_name_, ___expand_field_compare_statement);\
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
  };

////////////////////////////////////////////////////////////////////////////////
#define ___expand_define_set_field_method(fid_, fname_, ftype_, struct_name_) \
    void struct_name_::__set_##fname_(const ftype_& val) { this->fname_ = val;}

#define ___define_struct_self(struct_name_) \
    struct_name_::~struct_name_() throw() {} \
    \
    ___apply_expand(struct_name_, ___expand_define_set_field_method, struct_name_);\
    \

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
namespace msgrpc_demo {

    #define ___fields_of_struct___ResponseData(_, ...)\
      _(1, pet_id,     int32_t,           __VA_ARGS__)\
      _(2, pet_name,   std::string,       __VA_ARGS__)\
      _(3, pet_weight, int32_t,           __VA_ARGS__)

___def_struct(ResponseData);

} //namespace msgrpc_demo

////////////////////////////////////////////////////////////////////////////////

int main() {
    thrift::ResponseData ___foo;
    ___foo.pet_id = 11;
    ___foo.pet_name = "pet_name_foo";
    ___foo.pet_weight = 23;

    uint8_t* pbuf; uint32_t len;

    if (ThriftEncoder::encode(___foo, &pbuf, &len)) {
        std::string encoded_json((char*)pbuf);
        std::cout << encoded_json << std::endl;

        ___foo.pet_id = ___foo.pet_weight = 0;
        ___foo.pet_name = "aa";

        if (ThriftDecoder::decode(___foo, pbuf, len)) {
            cout << ___foo.pet_id << endl;
            cout << ___foo.pet_name << endl;
            cout << ___foo.pet_weight << endl;
        }
    }

    return 0;
}
