#ifndef PROJECT_SERVICE_FILTER_H
#define PROJECT_SERVICE_FILTER_H

#include <string>
#include <msgrpc/core/adapter/service_register.h>

namespace msgrpc {

    struct ServiceFilter {
        virtual ~ServiceFilter() = default;

        virtual instance_vector_t filter_service(const std::string& service_name, const instance_vector_t& instances) = 0;
    };

}

#endif //PROJECT_SERVICE_FILTER_H
