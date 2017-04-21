#include <iostream>

#include <api/y_api_interface_declare.h>
#include <adapter_example/details/msgrpc_test.h>
#include <msgrpc/core/service_interaction/si_base.h>

void run_next_testcase();

////////////////////////////////////////////////////////////////////////////////
using namespace service_y;
DEFINE_SI(SI_call_y_f1m1, YReq, YRsp) {
    return IY(ctxt).___f1m1(req);
}

void testcase_0000() {
    YReq yreq;
    yreq.yreqa = 100;

    auto* rsp_cell = SI_call_y_f1m1().run(yreq);

    if (rsp_cell != nullptr) {
        derive_final_action([](msgrpc::Cell<YRsp>& r) {
            std::__1::cout << "value of result:" << r.value().yrspa << std::__1::endl;
            run_next_testcase();
        }, rsp_cell);
    }
}

////////////////////////////////////////////////////////////////////////////////
void run_testcase(size_t index) {
    std::vector<std::function<void(void)>> testcases = {
        testcase_0000
    };

    if (index < testcases.size()) {
        std::cout << "-------------------" << std::endl << "[RUN] testcase: " << index << std::endl;
        testcases[index]();
    } else {
        std::cout << "------------------------" << std::endl << "[ALL testcases finished]" << std::endl;
        exit(0);
    }
}

void run_next_testcase() {
    static size_t index = 0;
    run_testcase(index++);
}

////////////////////////////////////////////////////////////////////////////////
int main() {
    std::cout << "[service_start_up] service_x_main" << std::endl;

    test_thread msg_loop_thread(x_service_id, run_next_testcase, not_drop_msg);

    return 0;
}
