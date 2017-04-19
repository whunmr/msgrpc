#ifndef MSGRPC_RSP_CELL_BASE_H
#define MSGRPC_RSP_CELL_BASE_H

#include <msgrpc/core/cell/rsp_sink.h>
#include <msgrpc/core/components/rsp_dispatcher.h>

//TODO: to .h and .cpp

namespace msgrpc {
    struct RspCellBase : RspSink {
        virtual ~RspCellBase() {
            if (has_seq_id_) {
                RspDispatcher::instance().remove_rsp_handler(seq_id_);
            }
        }

        virtual void reset_sequential_id() override {
            has_seq_id_ = false;
        }

        virtual void set_sequential_id(const rpc_sequence_id_t &seq_id) override {
            seq_id_ = seq_id;
            has_seq_id_ = true;
        }

        bool has_seq_id_ = false;
        rpc_sequence_id_t seq_id_ = {0};
    };
}

#endif //MSGRPC_RSP_CELL_BASE_H
