#include <service_discovery/service_version_filter.h>
#include <msgrpc/util/instances_collector.h>
#include <msgrpc/core/adapter/logger.h>

ServiceVersionFilter::ServiceVersionFilter() {
    msgrpc::InstancesCollector<ServiceFilter>::instance().track(*this);
}

msgrpc::instance_vector_t ServiceVersionFilter::filter_service(const std::string& service_name, const msgrpc::instance_vector_t& instances) {
    msgrpc::instance_vector_t iv;

    if (instances.size() > 1) {
        auto iter = instances.begin();
        ++iter;

        iv.insert(iv.end(), iter, instances.end());
    } else {
        iv.insert(iv.end(), instances.begin(), instances.end());
    }

    ___log_debug("hit service version filter:  we can filter service instances by version here. "\
                 "instances size before filter: %d, after filter %d", instances.size(), iv.size());

    return iv;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ServiceVersionFilter global_auto_register_instance;



