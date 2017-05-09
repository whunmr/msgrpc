#ifndef MSGRPC_RPC_TIMEOUT_HANDLER_H
#define MSGRPC_RPC_TIMEOUT_HANDLER_H

#include <cstddef>
#include <msgrpc/util/singleton.h>

namespace msgrpc {

    struct RpcTimeoutHandler : msgrpc::ThreadLocalSingleton<RpcTimeoutHandler> {
        void on_timeout(const char* msg, size_t len);
    };
}

#endif //MSGRPC_RPC_TIMEOUT_HANDLER_H
