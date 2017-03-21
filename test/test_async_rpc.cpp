#include <iostream>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;

#include "demo/demo_api_declare.h"

#if 0
    #define ___methods_of_interface___IBuzzMath(_, ...)            \
        _(1, ResponseBar, negative_fields, RequestFoo, __VA_ARGS__)\
        _(2, ResponseBar, plus1_to_fields, RequestFoo, __VA_ARGS__)

    ___as_interface(IBuzzMath)
#endif

using namespace demo;

////////////////////////////////////////////////////////////////////////////////
struct IBuzzMath {
    virtual ResponseBar negative_fields(const RequestFoo&) = 0;
    virtual ResponseBar plus1_to_fields(const RequestFoo&) = 0;
};

////////////////////////////////////////////////////////////////////////////////
#include "test_util/UdpChannel.h"

void local_rpc_stub() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    UdpChannel channel(3333,
        [&channel](const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                channel.send_msg_to_remote("hello", udp::endpoint(udp::v4(), 2222));
            } else {
                cout << "local received msg: " << msg << endl;
                channel.close();
            }
        }
    );
}

void remote_rpc_implement() {
    UdpChannel channel(2222,
        [&channel](const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                return;
            }
            cout << "remote received msg: " << msg << endl;
            time_t now = time(0);
            channel.send_msg_to_sender(ctime(&now));
            channel.close();
        }
    );
}


TEST(async_rpc, should_able_to__auto__register_rpc_interface__after__application_startup) {
    demo::RequestFoo req;
    req.fooa = 1;
    req.__set_foob(2);

    std::thread local_thread(local_rpc_stub);
    std::thread remote_thread(remote_rpc_implement);

    local_thread.join();
    remote_thread.join();
};
