#ifndef MSGRPC_CELL_H
#define MSGRPC_CELL_H

#include <utility>
#include <iostream>
#include <include/msgrpc/core/rpc_result.h>
#include <include/msgrpc/thrift_struct/thrift_codec.h>
#include <msgrpc/core/cell/cell_base.h>
#include <msgrpc/core/cell/rsp_cell_base.h>

//TODO: .h and .inc

namespace msgrpc {

    template<typename T>
    struct Cell : CellBase<T>, RspCellBase {
        template<typename C, typename... Ts>
        void register_as_listener(C &&c, Ts &&... args) {
            c.register_listener(this);
            register_as_listener(std::forward<Ts>(args)...);
        }

        template<typename C>
        void register_as_listener(C &&c) {
            c.register_listener(this);
        }

        void register_as_listener() {
        }

        virtual void set_rpc_rsp(const RspMsgHeader& rsp_header, const char* msg, size_t len) override {
            if (rsp_header.rpc_result_ != RpcResult::succeeded) {
                std::cout << "rsp_header->rpc_result_: " << (int)rsp_header.rpc_result_ << std::endl;
                CellBase<T>::set_failed_reason(rsp_header.rpc_result_);
                return;
            }

            assert((msg != nullptr && len > 0) && "should has payload when rpc succeeded.");

            T rsp;
            if (! ThriftDecoder::decode(rsp, (uint8_t *) msg, len)) {
                std::cout << "decode rpc response failed. rpc seq_id: [TODO: unique_global_id]" << std::endl;
                CellBase<T>::set_failed_reason(RpcResult::failed);
                return;
            }

            CellBase<T>::set_value(rsp);
        }

        virtual void set_timeout() override {
            CellBase<T>::set_failed_reason(RpcResult::timeout);
        }
    };

}

#endif //MSGRPC_CELL_H
