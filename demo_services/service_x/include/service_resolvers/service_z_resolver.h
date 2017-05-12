#ifndef PROJECT_SERVICE_Z_RESOLVER_H
#define PROJECT_SERVICE_Z_RESOLVER_H

#include <api/service_z/z_api_struct_declare.h>
#include <msgrpc/core/service_discovery/named_sr_listener.h>
#include <msgrpc/core/adapter/service_register.h>

struct Z__ServiceResolver : msgrpc::NamedSRListener<service_z::k_name>, msgrpc::NamedResolver<service_z::k_name>, msgrpc::Singleton<Z__ServiceResolver> {
    virtual msgrpc::optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        ___log_debug("service_name_to_id from Z__ServiceResolver");
        return boost::none;
    }

    virtual void on_changes(const msgrpc::instance_vector_t& instances) override {
        ___log_debug("Z__ServiceResolver::on_changes");
    }
};
auto p98 = Z__ServiceResolver::instance().register_as_listener();


#endif //PROJECT_SERVICE_Z_RESOLVER_H
