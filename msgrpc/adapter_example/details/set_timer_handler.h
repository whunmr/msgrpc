#ifndef MSGRPC_SET_TIMER_HANDLER_H
#define MSGRPC_SET_TIMER_HANDLER_H

#include <adapter_example/core/adapter/simple_timer_adapter.h>

#include <cassert>
#include <thread>

#include <msgrpc/util/singleton.h>
#include <adapter_example/details/UdpChannel.h>
#include <adapter_example/core/adapter/udp_msg_channel.h>
#include <adapter_example/core/adapter/simple_logger.h>

namespace demo {
    struct SetTimerHandler : msgrpc::ThreadLocalSingleton<SetTimerHandler> {
        void set_timer(const char* msg, size_t len) {
            static unsigned short entry_times = 0;
            ++entry_times;

            assert(msg != nullptr && len == sizeof(timer_info));
            timer_info& ti = *(timer_info*)msg;

            unsigned short temp_udp_port = 20000 + entry_times;

            std::thread timer_thread([ti, temp_udp_port]{
                msgrpc::Config::instance().init_with( &UdpMsgChannel::instance()
                                                    , &SimpleTimerAdapter::instance()
                                                    , &ZkServiceRegister::instance()
                                                    , &SimpleLogger::instance()
                                                    , k_msgrpc_request_msg_id
                                                    , k_msgrpc_response_msg_id
                                                    , k_msgrpc_set_timer_msg
                                                    , k_msgrpc_timeout_msg
                                                    , k_msgrpc_schedule_task_on_main_thread_msg);

                UdpChannel channel(msgrpc::service_id_t(boost::asio::ip::address::from_string("127.0.0.1"), temp_udp_port),
                                   [ti](msgrpc::msg_id_t msg_id, const char* msg, size_t len, udp::endpoint sender) {
                                       if (0 == strcmp(msg, "init")) {
                                           std::this_thread::sleep_for(std::chrono::milliseconds(ti.millionseconds_));
                                           msgrpc::Config::instance().msg_channel_->send_msg(ti.service_id_, k_msgrpc_timeout_msg, (const char*)&ti, sizeof(ti));
                                       }
                                   }
                );
            });
            timer_thread.detach();
        }
    };
}

#endif //MSGRPC_SET_TIMER_HANDLER_H
