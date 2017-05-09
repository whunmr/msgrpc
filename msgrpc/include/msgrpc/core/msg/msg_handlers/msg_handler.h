#ifndef PROJECT_MSG_HANDLER_H
#define PROJECT_MSG_HANDLER_H

#include <msgrpc/core/typedefs.h>
#include <msgrpc/util/singleton.h>

namespace msgrpc {

    struct MsgHandler {
        virtual ~MsgHandler() = default;

        /* return true, if msg has been handled, and should not continue try other handlers */
        virtual bool handle_msg(msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct MsgHandlerMgr : MsgHandler, Singleton<MsgHandlerMgr> {
        virtual bool handle_msg(msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id) override;

        void register_msg_handler(msg_id_t msg_id, MsgHandler* msg_handler);
    };

    ////////////////////////////////////////////////////////////////////////////////
    template<msg_id_t MSG_ID_>
    struct AutoRegisterMsgHandler : MsgHandler {
        AutoRegisterMsgHandler() {
            MsgHandlerMgr::instance().register_msg_handler(MSG_ID_, this);
        }
    };
}

#endif //PROJECT_MSG_HANDLER_H
