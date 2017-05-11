#ifndef PROJECT_SERVICE_REGISTER_H
#define PROJECT_SERVICE_REGISTER_H

#include <boost/optional.hpp>
#include <msgrpc/core/adapter/adapter_base.h>
#include <set>
#include <vector>

namespace msgrpc {

    typedef boost::optional<msgrpc::service_id_t> optional_service_id_t;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ServiceResolver {
        virtual ~ServiceResolver() = default;

        virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) = 0;
    };


    struct ServiceRegister : AdapterBase, ServiceResolver {
        virtual ~ServiceRegister() = default;

        virtual bool register_service(const char* service_name, const char* version, const char* end_point) = 0;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct InstanceInfo {
        msgrpc::service_id_t service_id_;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct instance_info_compare {
        bool operator() (const InstanceInfo &a, const InstanceInfo &b) const { return a.service_id_ < b.service_id_; }
    };

    typedef std::set<InstanceInfo, instance_info_compare> instance_set_t;
    typedef std::vector<InstanceInfo> instance_vector_t;
}

#endif //PROJECT_SERVICE_REGISTER_H
