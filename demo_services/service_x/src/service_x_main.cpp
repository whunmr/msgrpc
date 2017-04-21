#include <iostream>

#include <api/y_api_interface_declare.h>
#include <adapter_example/details/msgrpc_test.h>
#include <msgrpc/core/service_interaction/si_base.h>

////////////////////////////////////////////////////////////////////////////////
using namespace service_y;
DEFINE_SI(SI_call_y_f1m1, YReq, YRsp) {
    return IY(ctxt).___f1m1(req);
}

std::atomic_uint count {0};

void rpc_main();

void testcase_0000() {
    YReq yreq;
    yreq.yreqa = 100;

    auto* rsp_cell = SI_call_y_f1m1().run(yreq);

    if (rsp_cell != nullptr) {
        derive_final_action([](msgrpc::Cell<YRsp>& r) {
            //std::__1::cout << "value of result:" << r.value().yrspa << std::__1::endl;
            ++count;
            if (count % 100 == 0) {
                std::cout << count << std::endl;
            }
            if (count % 3000 == 0) {
                rpc_main();
            }
        }, rsp_cell);
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

////////////////////////////////////////////////////////////////////////////////
void rpc_main() {
    for (int j = 0; j < 3000; ++j){
        testcase_0000();
    }
}

////////////////////////////////////////////////////////////////////////////////
int main() {
    std::cout << "[service_start_up] service_x_main" << std::endl;

    test_thread msg_loop_thread(x_service_id, rpc_main, not_drop_msg);

    return 0;
}
