#include <iostream>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <msgrpc/thrift_struct/thrift_codec.h>

using namespace std;
using namespace std::chrono;

#include "demo/demo_api_declare.h"

#if 0
    #define ___methods_of_interface___IBuzzMath(_, ...)            \
        _(1, ResponseBar, negative_fields, RequestFoo, __VA_ARGS__)\
        _(2, ResponseBar, plus1_to_fields, RequestFoo, __VA_ARGS__)

    ___as_interface(IBuzzMath, __with_interface_id(1))
#endif


////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {
    template <typename T> struct Ret {};

    typedef unsigned short msg_id_t;
    typedef unsigned short service_id_t; //TODO: how to deal with different service id types

    struct MsgChannel {
        virtual uint32_t send_msg(const service_id_t& remote_service_id, msg_id_t msg_id, const char* buf, size_t len) const = 0;
    };

    struct Config {
        void initWith(MsgChannel* msg_channel, msgrpc::msg_id_t request_msg_id, msgrpc::msg_id_t response_msg_id) {
            instance().msg_channel_ = msg_channel;
            request_msg_id_  = request_msg_id;
            response_msg_id_ = response_msg_id;
        }

        static inline Config& instance() {
            static thread_local Config instance;
            return instance;
        }

        MsgChannel* msg_channel_;
        msg_id_t request_msg_id_;
        msg_id_t response_msg_id_;
    };
}

namespace msgrpc {
    /*TODO: using static_assert to assure name length of interface and method*/
    const size_t k_max_interface_name_len = 40;
    const size_t k_max_method_name_len = 40;

    /*TODO: consider make msgHeader encoded through thrift*/
    struct MsgHeader {
        unsigned char  msgrpc_version_;
        unsigned char  method_index_in_interface_;
        unsigned short interface_index_in_service_;
    };

    struct Request : MsgHeader {
    };

    struct RpcInvokeHandler {
        void handleInvoke(const MsgHeader& msg_header) {
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
#include "test_util/UdpChannel.h"
namespace demo {
    const msgrpc::msg_id_t k_msgrpc_request_msg_id = 101;
    const msgrpc::msg_id_t k_msgrpc_response_msg_id = 102;

    struct UdpMsgChannel : msgrpc::MsgChannel {
        virtual uint32_t send_msg(const msgrpc::service_id_t& remote_service_id, msgrpc::msg_id_t msg_id, const char* buf, size_t len) const {
            size_t msg_len_with_msgid = sizeof(msgrpc::msg_id_t) + len;
            char* mem = (char*)malloc(msg_len_with_msgid);
            if (mem) {
                *(msgrpc::msg_id_t*)(mem) = msg_id;
                memcpy(mem + sizeof(msgrpc::msg_id_t), buf, len);
                cout << "send msg len: " << msg_len_with_msgid << endl;
                g_msg_channel->send_msg_to_remote(string(mem, msg_len_with_msgid), udp::endpoint(udp::v4(), remote_service_id));
                free(mem);
            } else {
                cout << "send msg failed: allocation failure." << endl;
            }
            return 0;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
using namespace demo;

const msgrpc::service_id_t k_remote_service_id = 2222;
const msgrpc::service_id_t k_loacl_service_id  = 3333;

struct IBuzzMath {
    virtual msgrpc::Ret<ResponseBar> negative_fields(const RequestFoo&) = 0;
    virtual msgrpc::Ret<ResponseBar> plus1_to_fields(const RequestFoo&) = 0;
};

////////////////////////////////////////////////////////////////////////////////
struct IBuzzMathStub : IBuzzMath {
    virtual msgrpc::Ret<ResponseBar> negative_fields(const RequestFoo&);
    virtual msgrpc::Ret<ResponseBar> plus1_to_fields(const RequestFoo&);
};

msgrpc::Ret<ResponseBar> IBuzzMathStub::negative_fields(const RequestFoo& req) {
    uint8_t* pbuf; uint32_t len;
    /*TODO: extract interface for encode/decode for other protocol adoption such as protobuf*/
    if (!ThriftEncoder::encode(req, &pbuf, &len)) {
        /*TODO: how to do with log*/
        cout << "encode failed." << endl;
        return msgrpc::Ret<ResponseBar>();
    }

    //TODO: find k_remote_service_id by interface name "IBuzzMath"
    size_t msg_len_with_header = sizeof(msgrpc::MsgHeader) + len;

    char* mem = (char*)malloc(msg_len_with_header);
    if (!mem) {
        cout << "alloc mem failed, during sending rpc request." << endl;
        return msgrpc::Ret<ResponseBar>();
    }

    auto header = (msgrpc::MsgHeader*)mem;
    header->msgrpc_version_ = 0;
    header->interface_index_in_service_ = 1;
    header->method_index_in_interface_ = 1;
    memcpy(header + 1, (const char*)pbuf, len);

    cout << "stub sending msg with length: " << msg_len_with_header << endl;
    msgrpc::Config::instance().msg_channel_->send_msg(k_remote_service_id, k_msgrpc_request_msg_id, mem, msg_len_with_header);
    free(mem);
    return msgrpc::Ret<ResponseBar>();
}

msgrpc::Ret<ResponseBar> IBuzzMathStub::plus1_to_fields(const RequestFoo& req) {
    return msgrpc::Ret<ResponseBar>();
}

void local_service() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    msgrpc::Config::instance().initWith(new UdpMsgChannel(), k_msgrpc_request_msg_id, k_msgrpc_response_msg_id);

    UdpChannel channel(k_loacl_service_id,
        [&channel](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {

                IBuzzMathStub buzzMath;
                RequestFoo foo; foo.fooa = 97; foo.__set_foob(98);
                buzzMath.negative_fields(foo);

            } else {
                cout << "local received msg: " << string(msg, len) << endl;
                channel.close();
            }
        }
    );
}

////////////////////////////////////////////////////////////////////////////////
struct IBuzzMathImpl {
    bool onRpcInvoke(const msgrpc::MsgHeader& msg_header, const char* msg, size_t len, uint8_t*& pout_buf, uint32_t& out_buf_len);   //todo:remote_id
    bool do_negative_fields(const char* msg, size_t len, uint8_t*& pout_buf, uint32_t& out_buf_len);
    bool do_plus1_to_fields(const char* msg, size_t len, uint8_t*& pout_buf, uint32_t& out_buf_len) {/*TODO:*/ return false;}
};

bool IBuzzMathImpl::onRpcInvoke(const msgrpc::MsgHeader& msg_header, const char* msg, size_t len, uint8_t*& pout_buf, uint32_t& out_buf_len) {
    cout << (int)msg_header.msgrpc_version_ << endl;
    cout << (int)msg_header.interface_index_in_service_ << endl;
    cout << (int)msg_header.method_index_in_interface_ << endl;

    this->do_negative_fields(msg, len, pout_buf, out_buf_len);

    return true;
}

bool IBuzzMathImpl::do_negative_fields(const char* msg, size_t len, uint8_t*& pout_buf, uint32_t& out_buf_len) {
    RequestFoo req;
    if (!ThriftDecoder::decode(req, (uint8_t*)msg, len)) {
        cout << "decode failed on remote side." << endl;
        return false;
    }

    ResponseBar bar; /*TODO:change bar to inout parameter*/
    bar.__set_bara(req.get_foob());
    if (req.__isset.foob) {
        bar.__set_barb(req.fooa);
    }

    if (!ThriftEncoder::encode(bar, &pout_buf, &out_buf_len)) {
        cout << "encode failed on remtoe side." << endl;
        return false;
    }

    return true;
}

//ResponseBar IBuzzMathImpl::do_plus1_to_fields(const RequestFoo&, ResponseBar& bar) {
//    ResponseBar bar;
//    bar.__set_bara(1 + req.fooa);
//    if (req.__isset.foob) {
//        bar.__set_barb(1 + req.get_foob());
//    }
//    return bar;
//}

void remote_service() {
    msgrpc::Config::instance().initWith(new UdpMsgChannel(), k_msgrpc_request_msg_id, k_msgrpc_response_msg_id);

    UdpChannel channel(k_remote_service_id,
        [&channel](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                return;
            }
            cout << "remote received msg with length: " << len << endl;

            /*TODO: should first check msg_id == msgrpc_msg_request_id */
            if (len < sizeof(msgrpc::MsgHeader)) {
                cout << "invalid msg: without sufficient msg header info." << endl;
                return;
            }

            auto msg_header = (msgrpc::MsgHeader*)msg;
            msg += sizeof(msgrpc::MsgHeader);

            IBuzzMathImpl buzzMath;

            uint8_t* pout_buf; uint32_t out_buf_len;

            if (buzzMath.onRpcInvoke(*msg_header, msg, len - sizeof(msgrpc::MsgHeader), pout_buf, out_buf_len)) {
                /*TODO: send out msg with msgheader*/
                msgrpc::Config::instance().msg_channel_->send_msg(k_loacl_service_id, k_msgrpc_response_msg_id,(const char*)pout_buf, out_buf_len);
            }

            channel.close();
        }
    );
}

////////////////////////////////////////////////////////////////////////////////
TEST(async_rpc, should_able_to__auto__register_rpc_interface__after__application_startup) {
    demo::RequestFoo req;
    req.fooa = 1;
    req.__set_foob(2);

    std::thread local_thread(local_service);
    std::thread remote_thread(remote_service);

    local_thread.join();
    remote_thread.join();
};
