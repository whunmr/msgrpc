#ifndef MSGRPC_RPC_SEQUENCE_ID_H
#define MSGRPC_RPC_SEQUENCE_ID_H
#include <cstdint>
#include <msgrpc/util/singleton.h>

namespace msgrpc {
    typedef uint32_t rpc_sequence_id_t;

    const rpc_sequence_id_t k_invalid_rpc_sequence_id = 0;

    struct RpcSequenceId : msgrpc::Singleton<RpcSequenceId> {
        rpc_sequence_id_t get();
        void reset();
    };
}

#endif //MSGRPC_RPC_SEQUENCE_ID_H
