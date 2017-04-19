#ifndef MSGRPC_CONFIG_H
#define MSGRPC_CONFIG_H

#include <cassert>

#include <msgrpc/core/adapter/msg_channel.h>
#include <msgrpc/core/adapter/timer_adapter.h>

namespace msgrpc {
    struct Config {
        void init_with(MsgChannel* msg_channel, TimerAdapter* timer, msgrpc::msg_id_t request_msg_id, msgrpc::msg_id_t response_msg_id, msgrpc::msg_id_t set_timer_msg_id, msgrpc::msg_id_t timeout_msg_id) {
            assert(msg_channel != nullptr && timer != nullptr);

            msg_channel_ = msg_channel;
            timer_ = timer;
            request_msg_id_  = request_msg_id;
            response_msg_id_ = response_msg_id;
            set_timer_msg_id_ = set_timer_msg_id;
            timeout_msg_id_ = timeout_msg_id;
        }

        static inline Config& instance() {
            static thread_local Config instance;
            return instance;
        }

        MsgChannel* msg_channel_ = {nullptr};
        TimerAdapter* timer_ = {nullptr};
        msg_id_t request_msg_id_ = 0;
        msg_id_t response_msg_id_ = 0;
        msg_id_t set_timer_msg_id_ = 0;
        msg_id_t timeout_msg_id_ = 0;
    };
}

#endif //MSGRPC_CONFIG_H
