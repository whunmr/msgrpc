#include <service_resolvers/service_y_resolver.h>
#include <msgrpc/core/adapter/logger.h>

DEFINE_AND_REGISTER_SERVICE_RESOLVER(Y__ServiceResolver);

namespace {
    msgrpc::instance_vector_t instances_;
};

msgrpc::optional_service_id_t Y__ServiceResolver::service_name_to_id(const char* service_name, const char* req, size_t req_len) {
    ___log_debug("service_name_to_id from Y__ServiceResolver");

    size_t size = instances_.size();
    if (size > 0) {
        return instances_[size - 1].service_id_;
    }

    return boost::none;
}

void Y__ServiceResolver::on_changes(const msgrpc::instance_vector_t& instances) {
    ___log_debug("Y__ServiceResolver::on_changes, got instances count: %d", instances.size());
    instances_ = instances;
}

