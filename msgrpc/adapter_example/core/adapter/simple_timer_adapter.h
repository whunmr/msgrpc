#ifndef MSGRPC_DEMO_TIMER_ADAPTER_H
#define MSGRPC_DEMO_TIMER_ADAPTER_H

#include <msgrpc/core/adapter/timer_adapter.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include <msgrpc/util/singleton.h>
#include <msgrpc/core/rpc_sequence_id.h>
#include <msgrpc/adapter_example/details/test_constants.h>
#include <msgrpc/core/adapter/config.h>
#include <msgrpc/core/adapter/timer_adapter.h>

namespace demo {

    struct test_service : msgrpc::ThreadLocalSingleton<test_service> {
        msgrpc::service_id_t current_service_id_;
    };

    using msgrpc::timer_info;

    struct TimerMgr : msgrpc::ThreadLocalSingleton<TimerMgr> {
        void cancel_timer(msgrpc::rpc_sequence_id_t id) {
            canceled_timer_ids_.push_back(id);
        }

        bool should_ignore(const char* msg, size_t len) {
            assert(msg != nullptr && len == sizeof(timer_info));
            timer_info& ti = *(timer_info*)msg;

            msgrpc::rpc_sequence_id_t seq_id =  (msgrpc::rpc_sequence_id_t)((uintptr_t)(ti.user_data_));
            return std::find(canceled_timer_ids_.begin(), canceled_timer_ids_.end(), seq_id) != canceled_timer_ids_.end();
        }

        void reset() {
            canceled_timer_ids_.clear();
        }

        std::vector<msgrpc::rpc_sequence_id_t> canceled_timer_ids_;
    };

    struct SimpleTimerAdapter : msgrpc::TimerAdapter, msgrpc::Singleton<SimpleTimerAdapter> {
        virtual void set_timer(msgrpc::timeout_len_t millionseconds, msgrpc::msg_id_t timeout_msg_id, void* user_data) const override {
            msgrpc::service_id_t service_id = test_service::instance().current_service_id_;

            timer_info ti;
            ti.millionseconds_ = millionseconds;
            ti.service_id_ = test_service::instance().current_service_id_;
            ti.user_data_ = user_data;

            msgrpc::Config::instance().msg_channel_->send_msg(timer_service_id, timeout_msg_id, (const char*)&ti, sizeof(ti));
        }

        virtual void cancel_timer(msgrpc::msg_id_t timeout_msg_id, void* user_data) const override {
            std::cout << "[timer] cancel timer id: " << (msgrpc::rpc_sequence_id_t)((uintptr_t)(user_data)) << std::endl;
            demo::TimerMgr::instance().cancel_timer(msgrpc::rpc_sequence_id_t((uintptr_t)user_data));
        }
    };
}

#endif //MSGRPC_DEMO_TIMER_ADAPTER_H
