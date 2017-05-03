#include <iostream>
#include <api/y_api_struct_declare.h>
#include <adapter_example/details/msgrpc_test.h>
#include <random>

int main() {
    std::cout << "[service_start_up] service_y_main" << std::endl;

    srand (time(NULL));
    unsigned short port = rand() % 100 + 7777;

    auto init_y = [port] {
        string endpoint = string("127.0.0.1:") + std::to_string(port);
        service_y::msgrpc_register_service(endpoint.c_str());
    };

    const msgrpc::service_id_t y_service_id(boost::asio::ip::address::from_string("127.0.0.1"), port);
    test_thread msg_loop_thread(y_service_id, []{}, not_drop_msg);
    test_thread timer_thread(timer_service_id, init_y, not_drop_msg);

    return 0;
}
