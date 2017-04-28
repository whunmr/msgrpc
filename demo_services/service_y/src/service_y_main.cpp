#include <iostream>

#include <api/y_api_struct_declare.h>
#include <adapter_example/details/msgrpc_test.h>

int main() {
    std::cout << "[service_start_up] service_y_main" << std::endl;

    test_thread thread_timer(timer_service_id, []{}, not_drop_msg);
    test_thread msg_loop_thread(y_service_id, []{}, not_drop_msg);

    service_y::msgrpc_register_service();

    return 0;
}
