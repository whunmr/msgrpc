#ifndef MSGRPC_UDP_MSG_CHANNEL_H
#define MSGRPC_UDP_MSG_CHANNEL_H

#include <msgrpc/core/types.h>
#include <msgrpc/core/adapter/msg_channel.h>

#include <iostream>
#include <string>

#include <msgrpc/util/singleton.h>
#include <adapter_example/details/test_constants.h>

#include <adapter_example/details/UdpChannel.h>

namespace demo {

    struct UdpMsgChannel : msgrpc::MsgChannel, msgrpc::ThreadLocalSingleton<UdpMsgChannel> {
        virtual bool send_msg(const msgrpc::service_id_t& remote_service_id, msgrpc::msg_id_t msg_id, const char* buf, size_t len) const override {
            if (msg_id != k_msgrpc_set_timer_msg && msg_id != k_msgrpc_timeout_msg) {
                if (len > sizeof(msgrpc::MsgHeader)) {
                    msgrpc::MsgHeader* header = (msgrpc::MsgHeader*)buf;

                    if (remote_service_id.port() == 6666)
                        std::cout << "------>> X(" << header->sequence_id_ << ")"   << std::endl;
                    else if (remote_service_id.port() == 10000)
                        std::cout << "------>> Z(" << header->sequence_id_ << ")"   << std::endl;
                    else if (remote_service_id.port() == 7777)
                        std::cout << "------>> Y(" << header->sequence_id_ << ")"   << std::endl;
                    else
                        std::cout << "------>> " << remote_service_id.port() << "(" << header->sequence_id_ << ")" << std::endl;
                }
            }

            size_t msg_len_with_msgid = sizeof(msgrpc::msg_id_t) + len;
            char* mem = (char*)malloc(msg_len_with_msgid);
            if (mem) {
                *(msgrpc::msg_id_t*)(mem) = msg_id;
                memcpy(mem + sizeof(msgrpc::msg_id_t), buf, len);
                g_msg_channel->send_msg_to_remote(std::string(mem, msg_len_with_msgid), remote_service_id);
                free(mem);
            } else {
                std::cout << "send msg failed: allocation failure." << std::endl;
            }
            return true;
        }

        virtual bool send_msg_to_self(msgrpc::msg_id_t msg_id, const char* buf, size_t len) const override {
            msgrpc::service_id_t self_id = self();
            return send_msg(self_id, msg_id, buf, len);
        }

        virtual msgrpc::service_id_t sender() const override {
            assert(g_msg_channel->remote_endpoint_.port() != 0 && "can only call sender() during msg handling.");
            return g_msg_channel->remote_endpoint_;
        }

        virtual msgrpc::service_id_t self() const override {
            return g_msg_channel->service_id_;
        }
    };
}

#endif //MSGRPC_UDP_MSG_CHANNEL_H
