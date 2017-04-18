#ifndef MSGRPC_RPC_RESULT_H
#define MSGRPC_RPC_RESULT_H

namespace msgrpc {
    enum class RpcResult : unsigned short {
        succeeded = 0, failed = 1, timeout = 2, iface_not_found = 3, method_not_found = 4
    };
}

#endif //MSGRPC_RPC_RESULT_H
