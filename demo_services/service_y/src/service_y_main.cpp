#include <iostream>
#include <api/y_api_struct_declare.h>
#include <adapter_example/details/msgrpc_test.h>

int main() {
    std::cout << "[service_start_up] service_y_main" << std::endl;

    const msgrpc::service_id_t y_service_id(boost::asio::ip::address::from_string("127.0.0.1"), 8888);

    auto init_y = [] {
        service_y::msgrpc_register_service("127.0.0.1:8888");
    };

    test_thread msg_loop_thread(y_service_id, []{}, not_drop_msg);
    test_thread timer_thread(timer_service_id, init_y, not_drop_msg);

    return 0;
}
