#include <iostream>
#include <api/service_y/y_api_struct_declare.h>
#include <adapter_example/details/msgrpc_test.h>
#include <random>

int main() {
    std::cout << "[service_start_up] service_y_main" << std::endl;

    srand (time(NULL));
    unsigned short port = rand() % 100 + 7777;

    auto init_y = [port] {
        string endpoint = string("127.0.0.1:") + std::to_string(port);

        msgrpc::Config::instance().service_register_->init();
        service_y::msgrpc_register_service(endpoint.c_str());
    };

    const msgrpc::service_id_t y_service_id(boost::asio::ip::address::from_string("127.0.0.1"), port);
    msg_loop_thread msg_loop_thread(y_service_id, init_y, not_drop_msg);

    return 0;
}
