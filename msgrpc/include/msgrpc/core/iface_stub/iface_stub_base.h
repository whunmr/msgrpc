#ifndef MSGRPC_IFACE_STUB_BASE_H
#define MSGRPC_IFACE_STUB_BASE_H

#include <iostream>

#include <msgrpc/core/types.h>
#include <msgrpc/core/typedefs.h>
#include <msgrpc/core/cell/cell_base.h>
#include <msgrpc/core/cell/cell.h>
#include <msgrpc/core/adapter/config.h>
#include <msgrpc/core/iface_impl/iface_impl_utility.h>

namespace msgrpc {

    //TODO: split .h and .cpp
    struct IfaceStubBase {
        IfaceStubBase(RpcContext& ctxt) : ctxt_(ctxt) { }
        RpcContext& ctxt_;

        //TODO: split into .h and .cpp
        bool send_rpc_request_buf(const char* to_service_name
                                , msgrpc::iface_index_t iface_index
                                , msgrpc::method_index_t method_index
                                , const uint8_t *pbuf
                                , uint32_t len
                                , RspSink* rpc_rsp_cell_sink) const {

            size_t msg_len_with_header = sizeof(msgrpc::ReqMsgHeader) + len;

            char *mem = (char *) malloc(msg_len_with_header);
            if (!mem) {
                std::cout << "alloc mem failed, during sending rpc request." << std::endl;
                return false;
            }

            auto seq_id = msgrpc::RpcSequenceId::instance().get();
            msgrpc::RspMsgHandler::instance().register_rsp_Handler(seq_id, rpc_rsp_cell_sink);

            auto header = (msgrpc::ReqMsgHeader *) mem;
            header->msgrpc_version_ = 0;
            header->iface_index_in_service_ = iface_index;
            header->method_index_in_interface_ = method_index;
            header->sequence_id_ = seq_id;
            memcpy(header + 1, (const char *) pbuf, len);

            //std::cout << "stub sending msg with length: " << msg_len_with_header << std::endl;
            //TODO: find y_service_id by iface_impl name "IBuzzMath"
            boost::optional<msgrpc::service_id_t> service_id = msgrpc::Config::instance().service_register_->service_name_to_id(to_service_name, mem, msg_len_with_header);
            //msgrpc::service_id_t service_id = iface_index == 1 ? 6666 /*x_service_id*/ : 7777 /*y_service_id*/;

            if (!service_id) {
                std::cout << "[ERROR]can not find instance of service: " << to_service_name << std::endl;
                free(mem);
                return false;
            }

            msgrpc::msg_id_t req_msg_type = msgrpc::Config::instance().request_msg_id_;
            bool send_ret = msgrpc::Config::instance().msg_channel_->send_msg(service_id.value(), req_msg_type, mem, msg_len_with_header);
            free(mem);

            return send_ret;
        }

        template<typename REQ, typename RSP>
        Cell<RSP>* encode_request_and_send(const char* to_service_name, msgrpc::iface_index_t iface_index, msgrpc::method_index_t method_index, const REQ &req) const {
            uint8_t* pbuf;
            uint32_t len;
            /*TODO: extract iface_impl for encode/decode for other protocol adoption such as protobuf*/
            if (!ThriftEncoder::encode(req, &pbuf, &len)) {
                /*TODO: how to do with log, maybe should extract logging iface_impl*/
                std::cout << "encode failed." << std::endl;
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
