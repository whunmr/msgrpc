#ifndef MSGRPC_TYPES_H
#define MSGRPC_TYPES_H

#include <msgrpc/core/rpc_result.h>
#include <msgrpc/core/rpc_sequence_id.h>

namespace msgrpc {

    typedef uint8_t  method_index_t;
    typedef uint16_t iface_index_t;

    struct MsgHeader {
        uint8_t           msgrpc_version_               = {0};
        method_index_t    method_index_in_interface_    = {0};
        iface_index_t     iface_index_in_service_       = {0};
        rpc_sequence_id_t sequence_id_                  = {k_invalid_rpc_sequence_id};
        //TODO: unsigned char  feature_id_in_service_ = {0};
        //TODO: TLV encoded varient length options
        //TODO: if not encoded/decode, how to deal hton and ntoh
    };

    /*TODO: consider make msgHeader encoded through thrift*/
    struct ReqMsgHeader : MsgHeader {
    };

    struct RspMsgHeader : MsgHeader {
        RpcResult rpc_result_ = { RpcResult::succeeded };
    };
}


#endif //MSGRPC_TYPES_H
