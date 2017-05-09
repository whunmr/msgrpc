#include <msgrpc/core/msg/msg_handlers/rsp_msg_handler.h>

#include <msgrpc/core/adapter/logger.h>

namespace msgrpc {

    void RspMsgHandler::on_rsp_handler_timeout(rpc_sequence_id_t sequence_id) {
        auto iter = id_func_map_.find(sequence_id);
        if (iter == id_func_map_.end()) {
            ___log_warning(std::string("not existing handler to remove caused by timeout. seq id: ") + std::to_string(sequence_id));
            return;
        }

        RspSink *cell = (iter->second);

        cell->reset_sequential_id();
        id_func_map_.erase(iter);

        assert(cell != nullptr && "cell should be notnull when find in dispatch map.");
        cell->set_timeout();
    }

    void RspMsgHandler::remove_rsp_handler(rpc_sequence_id_t sequence_id) {
        auto iter = id_func_map_.find(sequence_id);
        if (iter == id_func_map_.end()) {
            ___log_warning(std::string("WARNING: not existing handler to remove: id: ") + std::to_string(sequence_id));
            return;
        }

        id_func_map_.erase(iter);
    }

    void RspMsgHandler::register_rsp_Handler(rpc_sequence_id_t sequence_id, RspSink *sink) {
        assert(sink != nullptr && "can not register null callback");
        assert(id_func_map_.find(sequence_id) == id_func_map_.end() && "should register with unique id.");
        id_func_map_[sequence_id] = sink;
        sink->set_sequential_id(sequence_id);
    }

    void RspMsgHandler::handle_rpc_rsp(msgrpc::msg_id_t msg_id, const char *msg, size_t len) {
        if (msg == nullptr) {
            ___log_warning(std::string("invalid rpc msg_handlers with msg == nullptr, msgid:") + std::to_string(msg_id));
            return;
        }

        if (len < sizeof(RspMsgHeader)) {
            //TODO: let ___log_warning support string format like: log("%s %s", a, b)
            ___log_warning(std::string("invalid msg payload len for msgid:") + std::to_string(msg_id) + " ,len:" + std::to_string(len));
            return;
        }

        auto *rsp_header = (RspMsgHeader *) msg;

        auto iter = id_func_map_.find(rsp_header->sequence_id_);
        if (iter == id_func_map_.end()) {
            ___log_warning(std::string("can not find rpc rsp handler for seq_id:") + std::to_string(rsp_header->sequence_id_));
            return;
        }

        (iter->second)->set_rpc_rsp(*rsp_header, msg + sizeof(RspMsgHeader), len - sizeof(RspMsgHeader));

        //if this msg_handlers finishes a SI, the handler (iter->second) will be release in whole SI context teardown;
        //otherwise, we should erase this very msg_handlers handler only.
        delete_rsp_handler_if_exist(rsp_header->sequence_id_);
    }

    void RspMsgHandler::delete_rsp_handler_if_exist(const rpc_sequence_id_t &seq_id) {
        auto iter = id_func_map_.find(seq_id);
        if (iter != id_func_map_.end()) {
            (iter->second)->reset_sequential_id();
            id_func_map_.erase(iter);
        }
    }

}
