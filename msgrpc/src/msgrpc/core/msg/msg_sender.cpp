#include <msgrpc/core/msg/msg_sender.h>

#include <msgrpc/core/adapter/config.h>

namespace msgrpc {

    void MsgSender::send_msg_with_header(const msgrpc::service_id_t& service_id, const RspMsgHeader& rsp_header, const uint8_t *pout_buf, uint32_t out_buf_len) {
        msg_id_t rsp_msg_type = Config::instance().response_msg_id_;

        if (pout_buf == nullptr || out_buf_len == 0) {
            Config::instance().msg_channel_->send_msg(service_id, rsp_msg_type, (const char*)&rsp_header, sizeof(rsp_header));
            return;
        }

        size_t rsp_len_with_header = sizeof(rsp_header) + out_buf_len;
        char *mem = (char *) malloc(rsp_len_with_header);
        if (mem != nullptr) {
            memcpy(mem, &rsp_header, sizeof(rsp_header));
            memcpy(mem + sizeof(rsp_header), pout_buf, out_buf_len);
            Config::instance().msg_channel_->send_msg(service_id, rsp_msg_type, mem, rsp_len_with_header);
            free(mem);
        }
    }

}
