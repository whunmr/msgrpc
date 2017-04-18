#ifndef MSGRPC_RSP_SINK_H
#define MSGRPC_RSP_SINK_H

#include <cstddef>
#include <msgrpc/core/rpc_sequence_id.h>

namespace msgrpc {

    struct RspMsgHeader;

    struct RspSink {
        virtual void set_rpc_rsp(const RspMsgHeader &rsp_header, const char *msg, size_t len) = 0;
        virtual void set_timeout() = 0;

        virtual void set_sequential_id(const rpc_sequence_id_t &seq_id) = 0;
        virtual void reset_sequential_id() = 0;
    };

}

#endif //MSGRPC_RSP_SINK_H
