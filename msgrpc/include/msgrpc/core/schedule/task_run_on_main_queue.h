#ifndef PROJECT_TASKRUNONMAINQUEUE_H
#define PROJECT_TASKRUNONMAINQUEUE_H

#include <msgrpc/core/adapter/config.h>
#include <adapter_example/details/test_constants.h>

namespace msgrpc {
    struct TaskRunOnMainQueue {
        struct TaskPtr {
            uintptr_t ptr_;
        };

        virtual ~TaskRunOnMainQueue() = default;
        virtual void run_task() const = 0;

        bool schedule() {
            TaskPtr taskptr;
            taskptr.ptr_ = (uintptr_t)this;

            return msgrpc::Config::instance().msg_channel_->send_msg_to_self( k_msgrpc_schedule_task_on_main_thread_msg
                                                                            , (const char*)&taskptr
                                                                            , sizeof(taskptr));
        }
    };
}

#endif //PROJECT_TASKRUNONMAINQUEUE_H
