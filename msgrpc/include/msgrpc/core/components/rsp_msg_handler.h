#ifndef MSGRPC_RSP_DISPATCHER_H
#define MSGRPC_RSP_DISPATCHER_H

#include <map>

#include <msgrpc/util/singleton.h>
#include <msgrpc/core/typedefs.h>
#include <msgrpc/core/types.h>
#include <msgrpc/core/cell/rsp_sink.h>

namespace msgrpc {

    struct RspMsgHandler : msgrpc::ThreadLocalSingleton<RspMsgHandler> {
        void on_rsp_handler_timeout(rpc_sequence_id_t sequence_id);

        void remove_rsp_handler(rpc_sequence_id_t sequence_id);

        void register_rsp_Handler(rpc_sequence_id_t sequence_id, RspSink *sink);

        void handle_rpc_rsp(msgrpc::msg_id_t msg_id, const char *msg, size_t len);

        void delete_rsp_handler_if_exist(const rpc_sequence_id_t &seq_id);

    private:
        std::map<rpc_sequence_id_t, RspSink *> id_func_map_;
    };

}
#endif //MSGRPC_RSP_DISPATCHER_H
