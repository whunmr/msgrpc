#ifndef PROJECT_SERVICE_Y_RESOLVER_H
#define PROJECT_SERVICE_Y_RESOLVER_H

#include <api/service_y/y_api_struct_declare.h>
#include <msgrpc/core/service_discovery/named_sr_listener.h>
#include <msgrpc/core/adapter/service_register.h>

struct Y__ServiceResolver : msgrpc::NamedSRListener<service_y::k_name>, msgrpc::NamedResolver<service_y::k_name>, msgrpc::Singleton<Y__ServiceResolver> {
    //TODO: add msgrpc::MsgHeader as header
    //TODO: add unencoded thrift struct as parameter
    virtual msgrpc::optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        ___log_debug("service_name_to_id from Y__ServiceResolver");
        return boost::none;
    }

    virtual void on_changes(const msgrpc::instance_vector_t& instances) override {
        ___log_debug("Y__ServiceResolver::on_changes");
    }
};
auto p86 = Y__ServiceResolver::instance().register_as_listener();

#endif //PROJECT_SERVICE_Y_RESOLVER_H
