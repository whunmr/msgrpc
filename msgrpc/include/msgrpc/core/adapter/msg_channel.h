#ifndef MSGRPC_MSG_CHANNEL_H
#define MSGRPC_MSG_CHANNEL_H

#include <msgrpc/core/typedefs.h>
#include <cstddef>
#include <msgrpc/core/adapter/adapter_base.h>

namespace msgrpc {

    struct MsgChannel : AdapterBase {
        virtual ~MsgChannel() = default;

        virtual bool send_msg(const service_id_t& remote_service_id, msg_id_t msg_id, const char* buf, size_t len) const = 0;
        virtual bool send_msg_to_self(msg_id_t msg_id, const char* buf, size_t len) const = 0;
        virtual msgrpc::service_id_t sender() const = 0;
        virtual msgrpc::service_id_t self() const = 0;
    };

}

#endif //MSGRPC_MSG_CHANNEL_H
