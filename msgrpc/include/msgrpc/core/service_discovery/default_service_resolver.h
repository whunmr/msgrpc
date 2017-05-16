#ifndef PROJECT_DEFAULT_SERVICE_RESOLVER_H
#define PROJECT_DEFAULT_SERVICE_RESOLVER_H

#include <msgrpc/core/service_discovery/service_resolver.h>
#include <msgrpc/util/singleton.h>
#include <msgrpc/core/adapter/logger.h>
#include <msgrpc/core/adapter/service_register.h>

namespace msgrpc {

    struct DefaultServiceResolver : ServiceResolver, Singleton<DefaultServiceResolver> {
        virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
            ___log_debug("using DefaultServiceResolver to resolve service %s", service_name);
            return msgrpc::Config::instance().service_register_->service_name_to_id(service_name, req, req_len);
        }
    };

}
#endif //PROJECT_DEFAULT_SERVICE_RESOLVER_H
