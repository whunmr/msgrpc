#ifndef MSGRPC_MSGRPC_TEST_LOOP_H
#define MSGRPC_MSGRPC_TEST_LOOP_H

#include <iostream>
#include <msgrpc/core/adapter/config.h>
#include <test/details/test_constants.h>
#include <test/core/adapter/simple_timer_adapter.h>
#include <test/core/adapter/udp_msg_channel.h>
#include <test/test_util/UdpChannel.h>
#include <msgrpc/core/components/req_msg_handler.h>
#include <msgrpc/core/components/rpc_timeout_handler.h>
#include <msgrpc/core/components/rsp_dispatcher.h>
#include <msgrpc/util/singleton.h>

void msgrpc_test_loop(unsigned short udp_port, std::function<void(void)> init_func, std::function<bool(const char *msg, size_t len)> should_drop) {
    msgrpc::Config::instance().init_with( &UdpMsgChannel::instance()
            , &SimpleTimerAdapter::instance()
            , k_msgrpc_request_msg_id
            , k_msgrpc_response_msg_id
            , k_msgrpc_set_timer_msg
            , k_msgrpc_timeout_msg);

    test_service::instance().current_service_id_ = udp_port;

    UdpChannel channel(udp_port,
                       [&init_func, udp_port, &should_drop](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
                           if (0 == strcmp(msg, "init")) {
                               return init_func();
                           } else if (msg_id == msgrpc::Config::instance().request_msg_id_) {
                               if (! should_drop(msg, len)) {
                                   return msgrpc::ReqMsgHandler::on_rpc_req_msg(msg_id, msg, len);
                               }
                           } else if (msg_id == msgrpc::Config::instance().response_msg_id_) {
                               return msgrpc::RspDispatcher::instance().handle_rpc_rsp(msg_id, msg, len);
                           } else if (msg_id == msgrpc::Config::instance().set_timer_msg_id_) {
                               return demo::SetTimerHandler::instance().set_timer(msg, len);
                           } else if (msg_id == msgrpc::Config::instance().timeout_msg_id_) {
                               if (! TimerMgr::instance().should_ignore(msg, len)) {
                                   return msgrpc::RpcTimeoutHandler::instance().on_timeout(msg, len);
                               }
                           } else {
                               std::cout << "got unknow msg with id: " << msg_id << std::endl;
                           }
                       }
    );
}


#endif //MSGRPC_MSGRPC_TEST_LOOP_H
