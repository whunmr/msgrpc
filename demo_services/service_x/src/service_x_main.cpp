#include <iostream>

//#include <api/export/x_api_struct_declare.h>
#include <adapter_example/details/msgrpc_test.h>

int main() {
    std::cout << "[service_start_up] service_x_main" << std::endl;

    test_thread msg_loop_thread(x_service_id, []{}, not_drop_msg);

    return 0;
}

