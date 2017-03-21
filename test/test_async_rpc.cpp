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

    ___as_interface(IBuzzMath)
#endif

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {
    template <typename T> struct Ret {};

    typedef unsigned short service_id_t;

    struct MsgChannel {
        virtual uint32_t send_msg(const service_id_t& remote_service_id, const char* buf, size_t len) const = 0;
    };
};

////////////////////////////////////////////////////////////////////////////////
#include "test_util/UdpChannel.h"
namespace demo {
    struct UdpMsgChannel : msgrpc::MsgChannel {
        virtual uint32_t send_msg(const msgrpc::service_id_t& remote_service_id, const char* buf, size_t len) const {
            g_msg_channel->send_msg_to_remote(string(buf, len), udp::endpoint(udp::v4(), remote_service_id));
            return 0;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
using namespace demo;
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

    return msgrpc::Ret<ResponseBar>();
}

msgrpc::Ret<ResponseBar> IBuzzMathStub::plus1_to_fields(const RequestFoo& req) {
    return msgrpc::Ret<ResponseBar>();
}

void local_service() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    UdpChannel channel(3333,
        [&channel](const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                g_msg_channel->send_msg_to_remote("hello", udp::endpoint(udp::v4(), 2222));
            } else {
                cout << "local received msg: " << msg << endl;
                channel.close();
            }
        }
    );
}

////////////////////////////////////////////////////////////////////////////////
void remote_service() {
    UdpChannel channel(2222,
        [&channel](const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                return;
            }
            cout << "remote received msg: " << string(msg, len) << endl;
            time_t now = time(0);
            g_msg_channel->send_msg_to_sender(ctime(&now));
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
