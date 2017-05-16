#ifndef PROJECT_SERVICE_VERSION_FILTER_H
#define PROJECT_SERVICE_VERSION_FILTER_H

#include <msgrpc/core/service_discovery/service_filter.h>

struct ServiceVersionFilter : msgrpc::ServiceFilter {
    ServiceVersionFilter();

    virtual msgrpc::instance_vector_t filter_service(const std::string& service_name, const msgrpc::instance_vector_t& instances) override;
};

#endif //PROJECT_SERVICE_VERSION_FILTER_H
