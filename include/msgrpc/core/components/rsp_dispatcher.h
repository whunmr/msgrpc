#ifndef MSGRPC_RSP_DISPATCHER_H
#define MSGRPC_RSP_DISPATCHER_H

#include <msgrpc/util/singleton.h>
#include <msgrpc/core/types.h>

namespace msgrpc {

    //TODO: split into .h and .cpp
    struct RspDispatcher : msgrpc::ThreadLocalSingleton<RspDispatcher> {
        void on_rsp_handler_timeout(rpc_sequence_id_t sequence_id) {
            auto iter = id_func_map_.find(sequence_id);
            if (iter == id_func_map_.end()) {
                std::cout << "WARNING: not existing handler to remove caused by timeout. seq id: " << sequence_id << std::endl;
                return;
            }

            RspSink *cell = (iter->second);

            cell->reset_sequential_id();
            id_func_map_.erase(iter);

            assert(cell != nullptr && "cell should be notnull when find in dispatch map.");
            cell->set_timeout();
        }

        void remove_rsp_handler(rpc_sequence_id_t sequence_id) {
            auto iter = id_func_map_.find(sequence_id);
            if (iter == id_func_map_.end()) {
                std::cout << "WARNING: not existing handler to remove: id: " << sequence_id << std::endl;
                return;
            }

            id_func_map_.erase(iter);
        }

        void register_rsp_Handler(rpc_sequence_id_t sequence_id, RspSink *sink) {
            assert(sink != nullptr && "can not register null callback");
            assert(id_func_map_.find(sequence_id) == id_func_map_.end() && "should register with unique id.");
            id_func_map_[sequence_id] = sink;
            sink->set_sequential_id(sequence_id);
        }

        void handle_rpc_rsp(msgrpc::msg_id_t msg_id, const char *msg, size_t len) {
            if (msg == nullptr) {
                std::cout << "invalid rpc components with msg == nullptr";
                return;
            }

            if (len < sizeof(RspMsgHeader)) {
                std::cout << "WARNING: invalid components msg" << std::endl;
                return;
            }

            auto *rsp_header = (RspMsgHeader *) msg;

            auto iter = id_func_map_.find(rsp_header->sequence_id_);
            if (iter == id_func_map_.end()) {
                std::cout << "WARNING: can not find components handler" << std::endl;
                return;
            }

            (iter->second)->set_rpc_rsp(*rsp_header, msg + sizeof(RspMsgHeader), len - sizeof(RspMsgHeader));

            //if this components finishes a SI, the handler (iter->second) will be release in whole SI context teardown;
            //otherwise, we should erase this very components handler only.
            delete_rsp_handler_if_exist(rsp_header->sequence_id_);
        }

        void delete_rsp_handler_if_exist(const rpc_sequence_id_t &seq_id) {
            auto iter = id_func_map_.find(seq_id);
            if (iter != id_func_map_.end()) {
                (iter->second)->reset_sequential_id();
                id_func_map_.erase(iter);
            }
        }

        std::map<rpc_sequence_id_t, RspSink *> id_func_map_;
    };

}
#endif //MSGRPC_RSP_DISPATCHER_H
