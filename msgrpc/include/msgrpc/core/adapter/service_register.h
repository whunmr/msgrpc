#ifndef PROJECT_SERVICE_REGISTER_H
#define PROJECT_SERVICE_REGISTER_H

#include <boost/optional.hpp>
#include <msgrpc/core/adapter/adapter_base.h>
#include <msgrpc/core/adapter/service_resolver.h>
#include <set>
#include <vector>

namespace msgrpc {

    struct InstanceInfo {
        msgrpc::service_id_t service_id_;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    typedef std::vector<InstanceInfo> instance_vector_t;

    struct ServiceRegisterListener {
        virtual ~ServiceRegisterListener() = default;

        virtual const char* service_to_listener() = 0;
        virtual void on_changes(const instance_vector_t& instances) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ServiceRegister : AdapterBase, ServiceResolver {
        virtual ~ServiceRegister() = default;

        virtual bool register_service(const char* service_name, const char* version, const char* end_point) = 0;

        virtual void   register_listener(ServiceRegisterListener& listener) = 0;
        virtual void unregister_listener(ServiceRegisterListener& listener) = 0;
    };

}

#endif //PROJECT_SERVICE_REGISTER_H
