#ifndef MSGRPC_RSP_CELL_BASE_H
#define MSGRPC_RSP_CELL_BASE_H

#include <msgrpc/core/cell/rsp_sink.h>

namespace msgrpc {

    struct RspCellBase : RspSink {
        virtual ~RspCellBase();

        virtual void reset_sequential_id() override;
        virtual void set_sequential_id(const rpc_sequence_id_t &seq_id) override;

        bool has_seq_id_ = false;
        rpc_sequence_id_t seq_id_ = {0};
    };

}

#endif //MSGRPC_RSP_CELL_BASE_H
