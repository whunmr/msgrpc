#include <msgrpc/core/msg/msg_handlers/main_queue_task_handler.h>
#include <msgrpc/core/schedule/task_run_on_main_queue.h>

namespace msgrpc {

    void MainQueueTaskHandler::handle_task(msgrpc::msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id) {
        if (len < sizeof(TaskRunOnMainQueue::TaskPtr)) {
            ___log_error("insufficient payload length of msg_id: %d", msg_id);
            return;
        }

        auto *taskptr = (TaskRunOnMainQueue::TaskPtr *) msg;
        TaskRunOnMainQueue *task = reinterpret_cast<TaskRunOnMainQueue *>(taskptr->ptr_);
        task->run_task();
        delete task;
    }

}