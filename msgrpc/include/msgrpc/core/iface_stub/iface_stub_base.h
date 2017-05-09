#ifndef MSGRPC_IFACE_STUB_BASE_H
#define MSGRPC_IFACE_STUB_BASE_H

#include <msgrpc/core/types.h>
#include <msgrpc/core/typedefs.h>
#include <msgrpc/core/cell/cell_base.h>
#include <msgrpc/core/cell/cell.h>
#include <msgrpc/core/iface_impl/iface_impl_utility.h>

namespace msgrpc {

    struct IfaceStubBase {
        IfaceStubBase(RpcContext& ctxt);

        RpcContext& ctxt_;

        bool send_rpc_request_buf( const char* to_service_name
                                 , msgrpc::iface_index_t iface_index
                                 , msgrpc::method_index_t method_index
                                 , const uint8_t *pbuf
                                 , uint32_t len
                                 , RspSink* rpc_rsp_cell_sink) const;

        template<typename REQ, typename RSP>
        Cell<RSP>* encode_request_and_send(const char* to_service_name, msgrpc::iface_index_t iface_index, msgrpc::method_index_t method_index, const REQ &req) const {
            uint8_t* pbuf;
            uint32_t len;

            if (!ThriftEncoder::encode(req, &pbuf, &len)) {
                ___log_error("encode buf failed.");
                return failed_cell_with_reason<RSP>(ctxt_, RpcResult::failed);
            }

            Cell<RSP>* rpc_result_cell = new Cell<RSP>();

            if (! send_rpc_request_buf(to_service_name, iface_index, method_index, pbuf, len, rpc_result_cell)) {
                delete rpc_result_cell;
                return failed_cell_with_reason<RSP>(ctxt_, RpcResult::failed);
            }

            return ctxt_.track(rpc_result_cell);
        };
    };
}

#endif //MSGRPC_IFACE_STUB_BASE_H
