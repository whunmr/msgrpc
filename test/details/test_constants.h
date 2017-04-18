#ifndef MSGRPC_TEST_CONSTANTS_H
#define MSGRPC_TEST_CONSTANTS_H

#include <include/msgrpc/core/typedefs.h>

const msgrpc::service_id_t x_service_id = 2222;
const msgrpc::service_id_t y_service_id = 3333;
const msgrpc::service_id_t timer_service_id = 5555;

const msgrpc::msg_id_t k_msgrpc_request_msg_id = 101;
const msgrpc::msg_id_t k_msgrpc_response_msg_id = 102;
const msgrpc::msg_id_t k_msgrpc_set_timer_msg = 103;
const msgrpc::msg_id_t k_msgrpc_timeout_msg = 104;


#endif //MSGRPC_TEST_CONSTANTS_H
