#ifndef MSGRPC_RPCSEQUENCEID_H
#define MSGRPC_RPCSEQUENCEID_H

#include <cstdint>

#include <msgrpc/util/singleton.h>

namespace msgrpc {
    typedef uint32_t rpc_sequence_id_t;

    const rpc_sequence_id_t k_invalid_rpc_sequence_id = 0;

    struct rpc_sequence_id : msgrpc::Singleton<rpc_sequence_id> {
        rpc_sequence_id_t get();
        void reset();
    };
}

#endif //MSGRPC_RPCSEQUENCEID_H
