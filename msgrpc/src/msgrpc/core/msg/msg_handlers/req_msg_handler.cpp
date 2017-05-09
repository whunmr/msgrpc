#include <msgrpc/core/msg/msg_handlers/req_msg_handler.h>

#include <msgrpc/core/iface_impl/iface_impl_base.h>
#include <msgrpc/core/adapter/config.h>
#include <msgrpc/core/adapter/msg_channel.h>
#include <msgrpc/core/msg/msg_sender.h>
#include <msgrpc/core/iface_impl/iface_repository.h>
#include <msgrpc/core/adapter/logger.h>

namespace msgrpc {

    void ReqMsgHandler::on_rpc_req_msg(msgrpc::msg_id_t msg_id, const char *msg, size_t len, const service_id_t& sender_id) {
        msg_id_t req_msg_type = Config::instance().request_msg_id_;

        assert(msg_id == req_msg_type && "invalid msg id for rpc");

        if (len < sizeof(msgrpc::ReqMsgHeader)) {
            ___log_warning("invalid msg: without sufficient msg header info.");
            return;
        }

        auto *req_header = (msgrpc::ReqMsgHeader *) msg;
        msg += sizeof(msgrpc::ReqMsgHeader);

        msgrpc::RspMsgHeader rsp_header;
        rsp_header.msgrpc_version_ = req_header->msgrpc_version_;
        rsp_header.iface_index_in_service_ = req_header->iface_index_in_service_;
        rsp_header.method_index_in_interface_ = req_header->method_index_in_interface_;
        rsp_header.sequence_id_ = req_header->sequence_id_;

        //msgrpc::service_id_t sender_id = msgrpc::Config::instance().msg_channel_->sender();

        IfaceImplBase *iface = IfaceRepository::instance().get_iface_impl_by(req_header->iface_index_in_service_);
        if (iface == nullptr) {
            rsp_header.rpc_result_ = RpcResult::iface_not_found;

            msg_id_t rsp_msg_type = Config::instance().response_msg_id_;
            msgrpc::Config::instance().msg_channel_->send_msg(sender_id, rsp_msg_type, (const char *) &rsp_header, sizeof(rsp_header));
            return;
        }

        RpcResult ret = iface->onRpcInvoke(*req_header, msg, len - sizeof(msgrpc::ReqMsgHeader), rsp_header, sender_id);

        if (ret == RpcResult::failed || ret == RpcResult::method_not_found) {
            return MsgSender::send_msg_with_header(sender_id, rsp_header, nullptr, 0);
        }

        //TODO: using pipelined processor to handling input/output msgheader and rpc statistics.
    }

}
