#ifndef MSGRPC_CONFIG_H
#define MSGRPC_CONFIG_H

#include <cassert>

#include <msgrpc/core/adapter/msg_channel.h>
#include <msgrpc/core/adapter/timer_adapter.h>
#include <msgrpc/core/adapter/service_register.h>
#include <msgrpc/core/adapter/logger.h>

namespace msgrpc {
    struct Config {
        void init_with( MsgChannel* msg_channel
                      , TimerAdapter* timer
                      , ServiceRegister* service_register
                      , Logger* logger
                      , msgrpc::msg_id_t request_msg_id
                      , msgrpc::msg_id_t response_msg_id
                      , msgrpc::msg_id_t set_timer_msg_id
                      , msgrpc::msg_id_t timeout_msg_id) {

            assert(msg_channel != nullptr && timer != nullptr && service_register != nullptr);

            msg_channel_ = msg_channel;
            timer_ = timer;
            service_register_ = service_register;
            logger_ = logger;

            request_msg_id_  = request_msg_id;
            response_msg_id_ = response_msg_id;
            set_timer_msg_id_ = set_timer_msg_id;
            timeout_msg_id_ = timeout_msg_id;
        }

        static inline Config& instance() {
            static
#ifdef USE_THREAD_SIMULATE_MSGRPC_PROCESS
            thread_local
#endif
            Config instance;

            return instance;
        }

        MsgChannel* msg_channel_ = {nullptr};
        TimerAdapter* timer_ = {nullptr};
        ServiceRegister* service_register_ = {nullptr};
        Logger* logger_ = {nullptr};

        msg_id_t request_msg_id_ = 0;
        msg_id_t response_msg_id_ = 0;
        msg_id_t set_timer_msg_id_ = 0;
        msg_id_t timeout_msg_id_ = 0;
    };
}

//////////////////////////////////////////////////////////////////////////////////////
#define ___msgrpc_log(level_, ...) \
do {\
    if (msgrpc::Config::instance().logger_->is_logging_for(msgrpc::Logger::level_)) {\
        msgrpc::Config::instance().logger_->log_##level_(__VA_ARGS__);\
    }\
} while(0)

#define ___log_trace(...)   ___msgrpc_log(trace,   __VA_ARGS__)
#define ___log_debug(...)   ___msgrpc_log(debug,   __VA_ARGS__)
#define ___log_info(...)    ___msgrpc_log(info,    __VA_ARGS__)
#define ___log_warning(...) ___msgrpc_log(warning, __VA_ARGS__)
#define ___log_error(...)   ___msgrpc_log(error,   __VA_ARGS__)
#define ___log_fatal(...)   ___msgrpc_log(fatal,   __VA_ARGS__)


#endif //MSGRPC_CONFIG_H
