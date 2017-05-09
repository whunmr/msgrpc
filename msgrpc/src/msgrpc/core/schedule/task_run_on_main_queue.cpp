#include <msgrpc/core/schedule/task_run_on_main_queue.h>

#include <msgrpc/core/adapter/config.h>

namespace msgrpc {

    bool TaskRunOnMainQueue::schedule() {
        TaskPtr taskptr;
        taskptr.ptr_ = (uintptr_t)this;

        msg_id_t sched_msg_id = msgrpc::Config::instance().schedule_task_on_main_thread_msg_id_;

        return msgrpc::Config::instance().msg_channel_->send_msg_to_self( sched_msg_id
                                                                        , (const char*)&taskptr
                                                                        , sizeof(taskptr));
    }

}