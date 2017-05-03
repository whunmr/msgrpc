#ifndef MSGRPC_MSGRPC_TEST_LOOP_H
#define MSGRPC_MSGRPC_TEST_LOOP_H

#include <iostream>
#include <msgrpc/core/adapter/config.h>
#include <adapter_example/details/test_constants.h>
#include <adapter_example/core/adapter/simple_timer_adapter.h>
#include <adapter_example/core/adapter/udp_msg_channel.h>
#include <adapter_example/core/adapter/zk_service_register.h>
#include <adapter_example/details/UdpChannel.h>
#include <msgrpc/core/components/req_msg_handler.h>
#include <msgrpc/core/components/rpc_timeout_handler.h>
#include <msgrpc/core/components/rsp_msg_handler.h>
#include <msgrpc/core/schedule/task_run_on_main_queue.h>
#include <msgrpc/util/singleton.h>
#include <adapter_example/details/set_timer_handler.h>

void msgrpc_test_loop(const msgrpc::service_id_t& service_id, std::function<void(void)> init_func, std::function<bool(const char *msg, size_t len)> should_drop) {
    msgrpc::Config::instance().init_with(&demo::UdpMsgChannel::instance()
            , &demo::SimpleTimerAdapter::instance()
            , &demo::ZkServiceRegister::instance()
            , k_msgrpc_request_msg_id
            , k_msgrpc_response_msg_id
            , k_msgrpc_set_timer_msg
            , k_msgrpc_timeout_msg);

    demo::test_service::instance().current_service_id_ = service_id;

    try {
        UdpChannel channel(service_id,
                           [&init_func, &should_drop](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
                               if (0 == strcmp(msg, "init")) {
                                   return init_func();
                               } else if (msg_id == msgrpc::Config::instance().request_msg_id_) {
                                   if (! should_drop(msg, len)) {
                                       //TODO: add msgrpc::MsgHandlerBase to auto register to a global map
                                       return msgrpc::ReqMsgHandler::on_rpc_req_msg(msg_id, msg, len);
                                   }
                               } else if (msg_id == msgrpc::Config::instance().response_msg_id_) {
                                   return msgrpc::RspMsgHandler::instance().handle_rpc_rsp(msg_id, msg, len);
                               } else if (msg_id == msgrpc::Config::instance().set_timer_msg_id_) {
                                   return demo::SetTimerHandler::instance().set_timer(msg, len);
                               } else if (msg_id == msgrpc::Config::instance().timeout_msg_id_) {
                                   if (! demo::TimerMgr::instance().should_ignore(msg, len)) {
                                       return msgrpc::RpcTimeoutHandler::instance().on_timeout(msg, len);
                                   }
                               } else if (msg_id == k_msgrpc_schedule_task_on_main_thread_msg) {
                                   auto* taskptr = (msgrpc::TaskRunOnMainQueue::TaskPtr*)msg;
                                   msgrpc::TaskRunOnMainQueue* task = reinterpret_cast<msgrpc::TaskRunOnMainQueue*>(taskptr->ptr_);
                                   task->run_task();
                                   delete task;
                               } else {
                                   std::cout << "got unknow msg with id: " << msg_id << std::endl;
                               }
                           }
        );
    } catch (...) {
        std::cout << "timer bind udp port failed, timer already existing on port: " << service_id.port() << std::endl;
        init_func();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
auto not_drop_msg = [](const char* msg, size_t len) {
    return false;
};

auto drop_all_msg = [](const char* msg, size_t len) {
    return true;
};

auto drop_msg_with_seq_id(std::initializer_list<int> seq_ids_to_drop) -> std::function<bool(const char*, size_t)> {
    return [seq_ids_to_drop](const char* msg, size_t len) -> bool {
        assert(len >= sizeof(msgrpc::ReqMsgHeader));
        msgrpc::ReqMsgHeader* req = (msgrpc::ReqMsgHeader*)(msg);
        for (int seq_to_drop : seq_ids_to_drop) {
            if (req->sequence_id_ == seq_to_drop) {
                return true;
            }
        }

        return false;
    };
}


#endif //MSGRPC_MSGRPC_TEST_LOOP_H
