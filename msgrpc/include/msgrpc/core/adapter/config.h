#ifndef MSGRPC_CONFIG_H
#define MSGRPC_CONFIG_H

#include <cassert>
#include <msgrpc/core/typedefs.h>

namespace msgrpc {
    struct MsgChannel;
    struct TimerAdapter;
    struct ServiceRegister;
    struct Logger;

    struct Config {
        static void switch_into_thread_local_mode();
        static bool is_thread_local_mode();
        static Config& instance();

        void init_with( MsgChannel* msg_channel
                      , TimerAdapter* timer
                      , ServiceRegister* service_register
                      , Logger* logger
                      , msgrpc::msg_id_t request_msg_id
                      , msgrpc::msg_id_t response_msg_id
                      , msgrpc::msg_id_t set_timer_msg_id
                      , msgrpc::msg_id_t timeout_msg_id
                      , msgrpc::msg_id_t schedule_task_on_main_thread_msg_id);

        MsgChannel* msg_channel_ = {nullptr};
        TimerAdapter* timer_ = {nullptr};
        ServiceRegister* service_register_ = {nullptr};
        Logger* logger_ = {nullptr};

        msg_id_t request_msg_id_ = 0;
        msg_id_t response_msg_id_ = 0;
        msg_id_t set_timer_msg_id_ = 0;
        msg_id_t timeout_msg_id_ = 0;
        msg_id_t schedule_task_on_main_thread_msg_id_ = 0;
    };

}

#endif //MSGRPC_CONFIG_H
