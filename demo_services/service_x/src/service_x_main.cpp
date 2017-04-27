#include <iostream>

#include <api/y_api_interface_declare.h>
#include <adapter_example/details/msgrpc_test.h>
#include <msgrpc/core/service_interaction/si_base.h>

#include <test_util/test_runner.h>
#include <msgrpc/core/cell/timeout_cell.h>

////////////////////////////////////////////////////////////////////////////////
using namespace service_y;
DEFINE_SI(SI_call_y_f1m1, YReq, YRsp) {
    auto call_y_f1m1 = [&ctxt, req]() {
        return IY(ctxt).___f1m1(req);
    };

    return ___rpc(___ms(10), call_y_f1m1);
}

DEF_TESTCASE(testcase_0001) {
    YReq yreq;
    yreq.yreqa = 100;

    auto* rsp_cell = SI_call_y_f1m1().run(yreq);

    if (rsp_cell != nullptr) {
        derive_final_action([](msgrpc::Cell<YRsp>& r) {
            EXPECT_EQ(true, r.has_value());
            EXPECT_EQ(200, r.value().yrspa);
            run_next_testcase();
        }, rsp_cell);
    }
}

DEF_TESTCASE(testcase_0002) {
    std::cout << "hello from testcase_0002" << std::endl;
    run_next_testcase();
}

////////////////////////////////////////////////////////////////////////////////
int main() {
    std::cout << "[service_start_up] service_x_main" << std::endl;

    test_thread thread_timer(timer_service_id, []{}, not_drop_msg);
    test_thread msg_loop_thread(x_service_id, run_next_testcase, not_drop_msg);

    return 0;
}
