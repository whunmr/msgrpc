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

    namespace details {
        template<typename LAMBDA>
        struct LambdaTaskRunOnMainQueue : msgrpc::TaskRunOnMainQueue {
            LambdaTaskRunOnMainQueue(const LAMBDA& lambda) : lambda_(lambda) {/**/}

            virtual void run_task() const override {
                lambda_();
            }

            LAMBDA lambda_;
        };
    }

    struct Task {
        template<typename LAMBDA>
        static bool schedule_run_on_main_queue(const LAMBDA& lambda) {
            auto* task = new details::LambdaTaskRunOnMainQueue<typename std::remove_reference<decltype(lambda)>::type>(lambda);

            if ( ! task->schedule()) {
                std::cout << "[ERROR] schedule task failed" << std::endl;
                delete task;
                return false;
            }

            return true;
        }
    };
}

#endif //PROJECT_TASKRUNONMAINQUEUE_H
