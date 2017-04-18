#ifndef MSGRPC_IFACE_IMPL_BASE_T_H
#define MSGRPC_IFACE_IMPL_BASE_T_H

#include <msgrpc/core/iface_impl/iface_repository.h>
#include <msgrpc/thrift_struct/thrift_codec.h>
#include <msgrpc/core/cell/cell.h>
#include <msgrpc/core/msg/msg_sender.h>

namespace msgrpc {

    template<typename RSP>
    static RpcResult send_rsp_cell_value(const service_id_t& sender_id, const RspMsgHeader &rsp_header, const Cell<RSP>& rsp_cell) {
        if (!rsp_cell.has_value_) {
            return RpcResult::failed;
        }

        uint8_t* pout_buf = nullptr;
        uint32_t out_buf_len = 0;
        if (!ThriftEncoder::encode(rsp_cell.value(), &pout_buf, &out_buf_len)) {
            std::cout << "encode failed on remtoe side." << std::endl;
            return RpcResult::failed;
        }

        MsgSender::send_msg_with_header(sender_id, rsp_header, pout_buf, out_buf_len);
        return RpcResult::succeeded;
    }

    template<typename T, iface_index_t iface_index>
    struct InterfaceImplBaseT : IfaceImplBase {
        InterfaceImplBaseT() {
            IfaceRepository::instance().add_iface_impl(iface_index, this);
        }

        template<typename REQ, typename RSP>
        RpcResult invoke_templated_method(msgrpc::Cell<RSP>* (T::*method_impl)(const REQ&)
                , const char *msg, size_t len
                , msgrpc::service_id_t& sender_id
                , msgrpc::RspMsgHeader& rsp_header) {

            REQ req;
            if (! ThriftDecoder::decode(req, (uint8_t *) msg, len)) {
                std::cout << "decode failed on remote side." << std::endl;
                return RpcResult::failed;
            }

            msgrpc::Cell<RSP>* rsp_cell = ((T*)this->*method_impl)(req);
            if ( rsp_cell == nullptr ) {
                //TODO: log call failed
                return RpcResult::failed;
            }

            if (rsp_cell->has_value_) {
                RpcResult ret = send_rsp_cell_value(sender_id, rsp_header, *rsp_cell);
                delete rsp_cell;
                return ret;
            }

            auto final_action = derive_final_action([sender_id, rsp_header](msgrpc::Cell<RSP>& r) {
                if (r.has_value_) {
                    send_rsp_cell_value(sender_id, rsp_header, r);
                } else {
                    //TODO: handle error case where result do not contains value. maybe timeout?
                }
            }, rsp_cell);

            return RpcResult::succeeded;
        }
    };
}

#endif //MSGRPC_IFACE_IMPL_BASE_T_H
