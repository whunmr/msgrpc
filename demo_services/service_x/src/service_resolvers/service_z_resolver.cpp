#include <service_resolvers/service_z_resolver.h>
#include <msgrpc/core/adapter/logger.h>

DEFINE_AND_REGISTER_SERVICE_RESOLVER(Z__ServiceResolver);

msgrpc::optional_service_id_t Z__ServiceResolver::service_name_to_id(const char* service_name, const char* req, size_t req_len) {
    ___log_debug("service_name_to_id from Z__ServiceResolver");
    return boost::none;
}

void Z__ServiceResolver::on_changes(const msgrpc::instance_vector_t& instances) {
    ___log_debug("Z__ServiceResolver::on_changes");
}
