#ifndef MSGRPC_THRIFT_CODEC_H_H
#define MSGRPC_THRIFT_CODEC_H_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

////////////////////////////////////////////////////////////////////////////////

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

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

#endif //MSGRPC_THRIFT_CODEC_H_H
