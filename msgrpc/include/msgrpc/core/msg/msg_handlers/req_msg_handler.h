#ifndef MSGRPC_REQ_MSG_HANDLER_H
#define MSGRPC_REQ_MSG_HANDLER_H

#include <msgrpc/core/typedefs.h>
#include "msg_handler_mgr.h"

namespace msgrpc {
    struct ReqMsgHandler {
        static void on_rpc_req_msg(msgrpc::msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id);
    };
}

#endif //MSGRPC_REQ_MSG_HANDLER_H
