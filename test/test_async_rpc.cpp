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
            g_msg_channel->send_msg_to_remote(string(buf, len), udp::endpoint(udp::v4(), remote_service_id));
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
    if (!ThriftEncoder::encode(req, &pbuf, &len)) {
        /*TODO: how to do with log*/
        cout << "encode failed." << endl;
        return msgrpc::Ret<ResponseBar>();
    }

    //TODO: find k_remote_service_id by interface name "IBuzzMath"
    size_t msg_len_with_header = sizeof(msgrpc::MsgHeader) + len;

    const char* mem = (const char*)malloc(msg_len_with_header);
    if (!mem) {
        cout << "alloc mem failed, during sending rpc request." << endl;
        return msgrpc::Ret<ResponseBar>();
    }

    msgrpc::MsgHeader* header = (msgrpc::MsgHeader*)mem;
    header->msgrpc_version_ = 0;
    header->interface_index_in_service_ = 1;
    header->method_index_in_interface_ = 1;
    memcpy(header + 1, (const char*)pbuf, len);

    msgrpc::Config::instance().msg_channel_->send_msg(k_remote_service_id, k_msgrpc_request_msg_id, mem, msg_len_with_header);

    return msgrpc::Ret<ResponseBar>();
}

msgrpc::Ret<ResponseBar> IBuzzMathStub::plus1_to_fields(const RequestFoo& req) {
    return msgrpc::Ret<ResponseBar>();
}

void local_service() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    msgrpc::Config::instance().initWith(new UdpMsgChannel(), k_msgrpc_request_msg_id, k_msgrpc_response_msg_id);

    UdpChannel channel(k_loacl_service_id,
        [&channel](const char* msg, size_t len) {
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
    void onRpcInvoke();   //todo:remote_id
    virtual ResponseBar negative_fields(const RequestFoo&);
    virtual ResponseBar plus1_to_fields(const RequestFoo&);
};

void IBuzzMathImpl::onRpcInvoke() {

}

ResponseBar IBuzzMathImpl::negative_fields(const RequestFoo& req) {
    ResponseBar bar; /*TODO:change bar to inout parameter*/
    bar.__set_bara(req.get_foob());
    if (req.__isset.foob) {
        bar.__set_barb(req.fooa);
    }
    return bar;
}

ResponseBar IBuzzMathImpl::plus1_to_fields(const RequestFoo& req) {
    ResponseBar bar;
    bar.__set_bara(1 + req.fooa);
    if (req.__isset.foob) {
        bar.__set_barb(1 + req.get_foob());
    }
    return bar;
}


void remote_service() {
    msgrpc::Config::instance().initWith(new UdpMsgChannel(), k_msgrpc_request_msg_id, k_msgrpc_response_msg_id);

    UdpChannel channel(k_remote_service_id,
        [&channel](const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                return;
            }
            cout << "remote received msg: " << string(msg, len) << endl;

            /*TODO: should first check msg_id == msgrpc_msg_request_id */
            if (len < sizeof(msgrpc::MsgHeader)) {
                cout << "invalid msg: without sufficient msg header info." << endl;
                return;
            }

            cout << (int)((msgrpc::MsgHeader*)(msg))->msgrpc_version_ << endl;
            cout << (int)((msgrpc::MsgHeader*)(msg))->interface_index_in_service_ << endl;
            cout << (int)((msgrpc::MsgHeader*)(msg))->method_index_in_interface_ << endl;

            msg += sizeof(msgrpc::MsgHeader);

            RequestFoo req;
            if (!ThriftDecoder::decode(req, (uint8_t*)msg, len)) {
                cout << "decode failed on remote side." << endl;
                channel.close();
                return;
            }

            IBuzzMathImpl buzzMath;
            ResponseBar bar = buzzMath.negative_fields(req);

            uint8_t* pbuf; uint32_t bar_len;
            if (!ThriftEncoder::encode(bar, &pbuf, &bar_len)) {
                cout << "encode failed on remtoe side." << endl;
                channel.close();
                return;
            }

            msgrpc::Config::instance().msg_channel_->send_msg(k_loacl_service_id, k_msgrpc_response_msg_id,(const char*)pbuf, bar_len);
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
