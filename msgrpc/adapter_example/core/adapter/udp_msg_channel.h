#ifndef MSGRPC_UDP_MSG_CHANNEL_H
#define MSGRPC_UDP_MSG_CHANNEL_H

#include <msgrpc/core/adapter/msg_channel.h>

#include <iostream>
#include <string>

#include <msgrpc/util/singleton.h>
#include <msgrpc/adapter_example/details/test_constants.h>

#include "msgrpc/adapter_example/details/UdpChannel.h"

namespace demo {

    struct UdpMsgChannel : msgrpc::MsgChannel, msgrpc::ThreadLocalSingleton<UdpMsgChannel> {
        virtual bool send_msg(const msgrpc::service_id_t& remote_service_id, msgrpc::msg_id_t msg_id, const char* buf, size_t len) const {
            if (msg_id != k_msgrpc_set_timer_msg && msg_id != k_msgrpc_timeout_msg) {
                std::cout << ((remote_service_id == x_service_id) ? "X <------ " : "   ------> Y") << std::endl;
            }

            size_t msg_len_with_msgid = sizeof(msgrpc::msg_id_t) + len;
            char* mem = (char*)malloc(msg_len_with_msgid);
            if (mem) {
                *(msgrpc::msg_id_t*)(mem) = msg_id;
                memcpy(mem + sizeof(msgrpc::msg_id_t), buf, len);
                g_msg_channel->send_msg_to_remote(std::string(mem, msg_len_with_msgid), udp::endpoint(udp::v4(), remote_service_id));
                free(mem);
            } else {
                std::cout << "send msg failed: allocation failure." << std::endl;
            }
            return true;
        }
    };
}

#endif //MSGRPC_UDP_MSG_CHANNEL_H
