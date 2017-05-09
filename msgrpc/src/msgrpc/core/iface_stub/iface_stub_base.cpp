#include <msgrpc/core/iface_stub/iface_stub_base.h>

#include <msgrpc/core/adapter/config.h>
#include <msgrpc/core/adapter/service_register.h>
#include <msgrpc/core/adapter/msg_channel.h>
#include <msgrpc/core/msg/msg_handlers/rsp_msg_handler.h>

namespace msgrpc {

    IfaceStubBase::IfaceStubBase(RpcContext& ctxt) : ctxt_(ctxt) { /**/ }


    bool IfaceStubBase::send_rpc_request_buf( const char* to_service_name
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

        boost::optional<msgrpc::service_id_t> service_id
                = msgrpc::Config::instance().service_register_->service_name_to_id(to_service_name, mem, msg_len_with_header);

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
}
