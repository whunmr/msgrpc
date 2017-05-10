#ifndef PROJECT_MSG_HANDLER_H
#define PROJECT_MSG_HANDLER_H

#include <msgrpc/core/typedefs.h>
#include <msgrpc/util/singleton.h>

namespace msgrpc {

    struct MsgHandlerMgr : Singleton<MsgHandlerMgr> {
        bool handle_msg(msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id);
    };

}

#endif //PROJECT_MSG_HANDLER_H
