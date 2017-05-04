#include <iostream>
#include <adapter_example/details/msgrpc_test.h>
#include <adapter_example/details/msgrpc_test_loop.h>

int main() {
    std::cout << "service_z_main" << std::endl;

    test_thread timer_thread(timer_service_id, []{}, not_drop_msg);

    return 0;
}
