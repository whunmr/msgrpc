#ifndef PROJECT_TASKRUNONMAINQUEUE_H
#define PROJECT_TASKRUNONMAINQUEUE_H

#include <msgrpc/core/adapter/logger.h>

namespace msgrpc {
    struct TaskRunOnMainQueue {
        struct TaskPtr {
            uintptr_t ptr_;
        };

        virtual ~TaskRunOnMainQueue() = default;
        virtual void run_task() const = 0;

        bool schedule();
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
        static bool dispatch_async_to_main_queue(const LAMBDA &lambda) {
            auto* task = new details::LambdaTaskRunOnMainQueue<typename std::remove_reference<decltype(lambda)>::type>(lambda);

            if ( ! task->schedule()) {
                ___log_error("[ERROR] schedule task failed");
                delete task;
                return false;
            }

            return true;
        }
    };
}

#endif //PROJECT_TASKRUNONMAINQUEUE_H
