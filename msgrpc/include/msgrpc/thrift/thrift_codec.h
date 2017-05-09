#ifndef MSGRPC_THRIFT_CODEC_H_H
#define MSGRPC_THRIFT_CODEC_H_H

#include <msgrpc/util/singleton.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <msgrpc/core/adapter/logger.h>

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {

    struct ThriftCodecBase {
        ThriftCodecBase() : mem_buf_(new apache::thrift::transport::TMemoryBuffer()),
                            protocol_(new apache::thrift::protocol::TBinaryProtocol(mem_buf_)) {}
                        //, protocol_(new TJSONProtocol(mem_buf_)) { }

    protected:
        boost::shared_ptr<apache::thrift::transport::TMemoryBuffer> mem_buf_;
        boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> protocol_;
        //boost::shared_ptr<apache::thrift::protocol::TJSONProtocol> protocol_;
    };

    struct ThriftEncoder : ThriftCodecBase, msgrpc::ThreadLocalSingleton<ThriftEncoder> {
        ThriftEncoder() : should_reset_to_default_size_(false) {}

        template<typename T>
        static bool encode(const T &___struct, uint8_t **buf, uint32_t *len) {
            return ThriftEncoder::instance().do_encode(___struct, buf, len);
        }

    private:
        template<typename T>
        bool do_encode(const T &___struct, uint8_t **buf, uint32_t *len) {
            *len = 0;

            try {
                if (should_reset_to_default_size_) {
                    mem_buf_->resetBuffer(apache::thrift::transport::TMemoryBuffer::defaultSize);
                } else {
                    mem_buf_->resetBuffer();
                }

                ___struct.write(protocol_.get());
                mem_buf_->getBuffer(buf, len);

                if (*len > apache::thrift::transport::TMemoryBuffer::defaultSize) {
                    should_reset_to_default_size_ = true;
                }
            } catch (const std::exception& ex) {
                ___log_error(ex.what());
                return false;
            }

            return *len != 0;
        }

        bool should_reset_to_default_size_;
    };


    struct ThriftDecoder : ThriftCodecBase, msgrpc::ThreadLocalSingleton<ThriftDecoder> {
        template<typename T>
        static bool decode(T &___struct, uint8_t *buf, uint32_t len) {
            ThriftDecoder::instance().mem_buf_->resetBuffer(buf, len,
                                                            apache::thrift::transport::TMemoryBuffer::MemoryPolicy::OBSERVE);
            return ThriftDecoder::instance().do_decode(___struct);
        }

    private:
        template<typename T>
        bool do_decode(T &___struct) {
            try {
                return ___struct.read(protocol_.get()) > 0;
            } catch (const std::exception& ex) {
                ___log_error(ex.what());
                return false;
            }
        }
    };

} //namespace msgrpc

////////////////////////////////////////////////////////////////////////////////

#endif //MSGRPC_THRIFT_CODEC_H_H
