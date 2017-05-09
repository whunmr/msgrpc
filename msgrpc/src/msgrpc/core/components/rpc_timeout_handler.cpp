#include <msgrpc/core/components/rpc_timeout_handler.h>

#include <cassert>

#include <msgrpc/core/adapter/timer_adapter.h>
#include <msgrpc/core/rpc_sequence_id.h>
#include <msgrpc/core/components/rsp_msg_handler.h>

namespace msgrpc {

    void RpcTimeoutHandler::on_timeout(const char* msg, size_t len) {
        assert(msg != nullptr && len == sizeof(timer_info));
        timer_info& ti = *(timer_info*)msg;

        msgrpc::rpc_sequence_id_t seq_id =  (msgrpc::rpc_sequence_id_t)((uintptr_t)(ti.user_data_));

        msgrpc::RspMsgHandler::instance().on_rsp_handler_timeout(seq_id);
    }

}
