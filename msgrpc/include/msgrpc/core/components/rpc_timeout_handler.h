#ifndef MSGRPC_RPC_TIMEOUT_HANDLER_H
#define MSGRPC_RPC_TIMEOUT_HANDLER_H

#include <cassert>

#include <msgrpc/util/singleton.h>
#include <msgrpc/core/adapter/timer_adapter.h>
#include <msgrpc/core/rpc_sequence_id.h>
#include <msgrpc/core/components/rsp_msg_handler.h>

namespace msgrpc {

    struct RpcTimeoutHandler : msgrpc::ThreadLocalSingleton<RpcTimeoutHandler> {
        void on_timeout(const char* msg, size_t len) {
            assert(msg != nullptr && len == sizeof(timer_info));
            timer_info& ti = *(timer_info*)msg;

            msgrpc::rpc_sequence_id_t seq_id =  (msgrpc::rpc_sequence_id_t)((uintptr_t)(ti.user_data_));

            msgrpc::RspMsgHandler::instance().on_rsp_handler_timeout(seq_id);
        }
    };
}

#endif //MSGRPC_RPC_TIMEOUT_HANDLER_H
