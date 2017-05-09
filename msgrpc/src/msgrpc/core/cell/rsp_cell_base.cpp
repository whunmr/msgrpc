#include <msgrpc/core/cell/rsp_cell_base.h>

#include <msgrpc/core/msg/msg_handlers/rsp_msg_handler.h>

namespace msgrpc {

    RspCellBase::~RspCellBase() {
        if (has_seq_id_) {
            RspMsgHandler::instance().remove_rsp_handler(seq_id_);
        }
    }

    void RspCellBase::reset_sequential_id() {
        has_seq_id_ = false;
    }

    void RspCellBase::set_sequential_id(const rpc_sequence_id_t &seq_id) {
        seq_id_ = seq_id;
        has_seq_id_ = true;
    }

}
