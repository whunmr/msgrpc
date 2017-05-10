#include <msgrpc/core/msg/msg_handlers/msg_handler_mgr.h>

#include <msgrpc/core/adapter/config.h>
#include <msgrpc/core/msg/msg_handlers/req_msg_handler.h>
#include <msgrpc/core/msg/msg_handlers/rsp_msg_handler.h>
#include <msgrpc/core/msg/msg_handlers/rpc_timeout_handler.h>
#include <msgrpc/core/msg/msg_handlers/main_queue_task_handler.h>

namespace msgrpc {

    bool MsgHandlerMgr::handle_msg(msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id){
        
        if (msg_id == Config::instance().request_msg_id_) {
            ReqMsgHandler::on_rpc_req_msg(msg_id, msg, len, sender_id);
            return true;

        } else if (msg_id == Config::instance().response_msg_id_) {
            RspMsgHandler::instance().handle_rpc_rsp(msg_id, msg, len);
            return true;

        } else if (msg_id == Config::instance().timeout_msg_id_) {
            RpcTimeoutHandler::instance().on_timeout(msg, len);
            return true;

        } else if (msg_id == Config::instance().schedule_task_on_main_thread_msg_id_) {
            MainQueueTaskHandler::instance().handle_task(msg_id, msg, len, sender_id);
            return true;
        }

        return false;
    }

}