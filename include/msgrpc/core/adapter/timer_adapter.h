#ifndef MSGRPC_TIMER_ADAPTER_H
#define MSGRPC_TIMER_ADAPTER_H

#include <msgrpc/core/typedefs.h>

namespace msgrpc {

    struct TimerAdapter {
        virtual void set_timer(long long millionseconds, msgrpc::msg_id_t timeout_msg_id, void *user_data) const = 0;
        virtual void cancel_timer(msgrpc::msg_id_t timeout_msg_id, void *user_data) const = 0;
    };

}

#endif //MSGRPC_TIMER_ADAPTER_H
