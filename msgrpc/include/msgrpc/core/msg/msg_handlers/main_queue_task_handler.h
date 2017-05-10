#ifndef PROJECT_MAIN_QUEUE_TASK_HANDLER_H
#define PROJECT_MAIN_QUEUE_TASK_HANDLER_H

#include <msgrpc/core/typedefs.h>
#include <msgrpc/util/singleton.h>

namespace msgrpc {

    struct MainQueueTaskHandler : Singleton<MainQueueTaskHandler> {
        static void handle_task(msgrpc::msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id);
    };

}

#endif //PROJECT_MAIN_QUEUE_TASK_HANDLER_H
