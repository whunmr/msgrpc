#ifndef MSGRPC_IFACE_IMPL_BASE_H
#define MSGRPC_IFACE_IMPL_BASE_H
#include <cstddef>

#include <msgrpc/core/rpc_result.h>
#include <msgrpc/core/types.h>
#include <msgrpc/core/typedefs.h>

namespace msgrpc {
    struct IfaceImplBase {
        virtual ~IfaceImplBase() {}
        virtual msgrpc::RpcResult onRpcInvoke( const msgrpc::ReqMsgHeader& msg_header
                , const char* msg, size_t len
                , msgrpc::RspMsgHeader& rsp_header
                , msgrpc::service_id_t& sender_id) = 0;
    };
}

#endif //MSGRPC_IFACE_IMPL_BASE_H
