#include <iostream>
#include <adapter_example/details/msgrpc_test.h>
#include <api/service_k/k_api_struct_declare.h>

int main() {
    std::cout << "service_k_main" << std::endl;

    ////////////////////////////////////////////////////////////////////////////////
    auto init_k = [] {
        msgrpc::Config::instance().service_register_->init();
        service_k::msgrpc_register_service("127.0.0.1:12000");

        ___log_info("init in service_k_main.");
    };

    const msgrpc::service_id_t k_service_id(boost::asio::ip::address::from_string("127.0.0.1"), 12000);
    msg_loop_thread loop_thread(k_service_id, init_k, not_drop_msg);


    return 0;
}
