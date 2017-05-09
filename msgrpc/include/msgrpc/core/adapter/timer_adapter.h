#ifndef MSGRPC_TIMER_ADAPTER_H
#define MSGRPC_TIMER_ADAPTER_H

#include <msgrpc/core/typedefs.h>
#include <msgrpc/core/adapter/adapter_base.h>

namespace msgrpc {

    struct timer_info {
        timeout_len_t millionseconds_;
        msgrpc::service_id_t service_id_;
        void *user_data_;
    };

    struct TimerAdapter : AdapterBase {
        virtual ~TimerAdapter() = default;

        virtual void set_timer(timeout_len_t millionseconds, msg_id_t timeout_msg_id, void *user_data) const = 0;
        virtual void cancel_timer(msg_id_t timeout_msg_id, void *user_data) const = 0;
    };
}

#endif //MSGRPC_TIMER_ADAPTER_H
