#include <msgrpc/core/rpc_sequence_id.h>

#include <atomic>

namespace {
    std::atomic_uint sequence_id_(msgrpc::k_invalid_rpc_sequence_id);
}

namespace msgrpc {
    rpc_sequence_id_t rpc_sequence_id::get() {
        ++sequence_id_;

        //skip value of k_invalid_rpc_sequence_id
        if (sequence_id_ == k_invalid_rpc_sequence_id) {
            ++sequence_id_;
        }

        return sequence_id_;
    }

    void rpc_sequence_id::reset() {
        sequence_id_ = k_invalid_rpc_sequence_id;
    }
}