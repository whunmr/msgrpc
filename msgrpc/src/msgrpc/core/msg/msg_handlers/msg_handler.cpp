#include <msgrpc/core/msg/msg_handlers/msg_handler.h>
#include <map>

namespace msgrpc {

    typedef std::map<msg_id_t, MsgHandler*> msg_map_t;

    msg_map_t& get_msg_handler_map() {
        static msg_map_t msg_map;
        return msg_map;
    }

    bool MsgHandlerMgr::handle_msg(msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id){
        msg_map_t& ___m = get_msg_handler_map();

        auto iter = ___m.find(msg_id);
        if (iter != ___m.end()) {
            return (iter->second)->handle_msg(msg_id, msg, len, sender_id);
        }

        //msg not handled in msgrpc's msg handlers, should continue handle msg to application's handlers.
        return false;
    }

    void MsgHandlerMgr::register_msg_handler(msg_id_t msg_id, MsgHandler* msg_handler) {
        msg_map_t& ___m = get_msg_handler_map();

        assert(msg_handler != nullptr && "msg_handler should not be null");
        assert(___m.find(msg_id) == ___m.end() && "msg_handler can only register once");

        ___m[msg_id] = msg_handler;
    }
}