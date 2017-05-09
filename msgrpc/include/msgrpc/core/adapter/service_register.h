#ifndef PROJECT_SERVICE_REGISTER_H
#define PROJECT_SERVICE_REGISTER_H

#include <boost/optional.hpp>
#include <msgrpc/core/adapter/adapter_base.h>

namespace msgrpc {

    struct ServiceRegister : AdapterBase {
        virtual ~ServiceRegister() = default;

        virtual bool register_service(const char* service_name, const char* version, const char* end_point) = 0;
        virtual boost::optional<service_id_t> service_name_to_id(const char* service_name, const char* req, size_t req_len) = 0;
    };

}

#endif //PROJECT_SERVICE_REGISTER_H
