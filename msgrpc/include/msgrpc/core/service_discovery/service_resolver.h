#ifndef PROJECT_SERVICE_RESOLVER_H
#define PROJECT_SERVICE_RESOLVER_H

#include <msgrpc/core/typedefs.h>
#include <boost/optional.hpp>

namespace msgrpc {

    typedef boost::optional<msgrpc::service_id_t> optional_service_id_t;

    struct InstanceInfo {
        msgrpc::service_id_t service_id_;
        std::string version_;
        //TODO: add metadata
    };

    typedef std::vector<InstanceInfo> instance_vector_t;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ServiceResolver {
        virtual ~ServiceResolver() = default;

        //TODO: add msgrpc::MsgHeader as header
        //TODO: add unencoded thrift struct as parameter

        virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) = 0;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<const char* SERVICE_NAME>
    struct NamedResolver : ServiceResolver {
        static const char* service_name_to_resolve_;
    };

    template<const char* SERVICE_NAME>
    const char* NamedResolver<SERVICE_NAME>::service_name_to_resolve_ = SERVICE_NAME;

}

#endif //PROJECT_SERVICE_RESOLVER_H
