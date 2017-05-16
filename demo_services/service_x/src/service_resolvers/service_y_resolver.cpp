#include <service_resolvers/service_y_resolver.h>
#include <msgrpc/core/adapter/logger.h>

msgrpc::SRListener* Y__ServiceResolver::instance_ptr = Y__ServiceResolver::instance().register_as_listener();
Y__ServiceResolver::Y__ServiceResolver() {
    auto force_reference = instance_ptr;
}

msgrpc::optional_service_id_t Y__ServiceResolver::service_name_to_id(const char* service_name, const char* req, size_t req_len) {
    ___log_debug("service_name_to_id from Y__ServiceResolver");
    return boost::none;
}

void Y__ServiceResolver::on_changes(const msgrpc::instance_vector_t& instances) {
    ___log_debug("Y__ServiceResolver::on_changes");
}

