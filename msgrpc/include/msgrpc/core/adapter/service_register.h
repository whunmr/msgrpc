#ifndef PROJECT_SERVICE_REGISTER_H
#define PROJECT_SERVICE_REGISTER_H

#include <boost/optional.hpp>

namespace msgrpc {

    //TODO: extract to component init/finit methods, currently has components: service register, timer_adapter, msg_channel, etc.
    struct ServiceRegister {
        virtual ~ServiceRegister() = default;
        virtual bool init() = 0;
        virtual bool register_service(const char* service_name, const char* version, const char* end_point) = 0;
        virtual boost::optional<service_id_t> service_name_to_id(const char* service_name, const char* req, size_t req_len) = 0;
    };

}

#endif //PROJECT_SERVICE_REGISTER_H
