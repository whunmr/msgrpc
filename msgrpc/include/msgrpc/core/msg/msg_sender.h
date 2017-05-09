#ifndef MSGRPC_MSG_SENDER_H
#define MSGRPC_MSG_SENDER_H

#include <msgrpc/core/typedefs.h>
#include <msgrpc/core/types.h>

namespace msgrpc {
    struct MsgSender {
        static void send_msg_with_header(const msgrpc::service_id_t& service_id, const RspMsgHeader& rsp_header, const uint8_t *pout_buf, uint32_t out_buf_len);
    };
}

#endif //MSGRPC_MSG_SENDER_H
