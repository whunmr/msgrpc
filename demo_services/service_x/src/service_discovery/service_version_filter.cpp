#include <service_discovery/service_version_filter.h>
#include <msgrpc/util/instances_collector.h>
#include <msgrpc/core/adapter/logger.h>

ServiceVersionFilter::ServiceVersionFilter() {
    msgrpc::InstancesCollector<ServiceFilter>::instance().track(*this);
}

msgrpc::instance_vector_t ServiceVersionFilter::filter_service(const std::string& service_name, const msgrpc::instance_vector_t& instances) {
    ___log_debug("hit service filter:  we can filter service instances by version here.");
    return instances;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ServiceVersionFilter global_auto_register_instance;



