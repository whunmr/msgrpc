#include <iostream>

#include <api/y_api_struct_declare.h>
#include <adapter_example/details/msgrpc_test.h>

int main() {
    std::cout << "[service_start_up] service_y_main" << std::endl;

    test_thread msg_loop_thread(y_service_id, []{}, not_drop_msg);

    return 0;
}
