#include <msgrpc/core/adapter/config.h>

namespace msgrpc {

    bool is_thread_local_mode = false;
    void Config::switch_into_thread_local_mode() {
        is_thread_local_mode = true;
    }

    Config& Config::instance() {
        static thread_local Config thread_local_instance;
        static              Config global_instance;

        return is_thread_local_mode ? thread_local_instance : global_instance;
    }

    void Config::init_with( MsgChannel* msg_channel
                          , TimerAdapter* timer
                          , ServiceRegister* service_register
                          , Logger* logger
                          , msgrpc::msg_id_t request_msg_id
                          , msgrpc::msg_id_t response_msg_id
                          , msgrpc::msg_id_t set_timer_msg_id
                          , msgrpc::msg_id_t timeout_msg_id
                          , msgrpc::msg_id_t schedule_task_on_main_thread_msg_id) {

        assert(msg_channel != nullptr && timer != nullptr && service_register != nullptr);

        msg_channel_ = msg_channel;
        timer_ = timer;
        service_register_ = service_register;
        logger_ = logger;

        request_msg_id_  = request_msg_id;
        response_msg_id_ = response_msg_id;
        set_timer_msg_id_ = set_timer_msg_id;
        timeout_msg_id_ = timeout_msg_id;
        schedule_task_on_main_thread_msg_id_ = schedule_task_on_main_thread_msg_id;
    }

}