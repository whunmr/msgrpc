#include <iostream>
#include <adapter_example/details/msgrpc_test.h>
#include <api/z_api_struct_declare.h>

int main() {
    std::cout << "service_z_main" << std::endl;

    test_thread timer_thread(timer_service_id, []{}, not_drop_msg);


    auto init_y = [] {
        msgrpc::Config::instance().service_register_->init();
        service_z::msgrpc_register_service("127.0.0.1:10000");
    };

    const msgrpc::service_id_t y_service_id(boost::asio::ip::address::from_string("127.0.0.1"), 10000);
    test_thread msg_loop_thread(y_service_id, init_y, not_drop_msg);

    return 0;
}

