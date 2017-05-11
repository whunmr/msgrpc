#ifndef PROJECT_SERVICE_RESOLVER_H
#define PROJECT_SERVICE_RESOLVER_H

namespace msgrpc {

    typedef boost::optional<msgrpc::service_id_t> optional_service_id_t;

    struct ServiceResolver {
        virtual ~ServiceResolver() = default;

        virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) = 0;
    };
}

#endif //PROJECT_SERVICE_RESOLVER_H
