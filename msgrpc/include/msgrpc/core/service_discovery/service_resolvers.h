#ifndef PROJECT_COMBINED_RESOLVER_H
#define PROJECT_COMBINED_RESOLVER_H

namespace msgrpc {

    template<typename... RESOLVER>
    struct ServiceResolvers : ServiceResolver , Singleton<ServiceResolvers<RESOLVER...>> {

        ServiceResolvers();
        virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override;

      private:
        std::map<std::string, ServiceResolver*> resolvers_;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename... RESOLVER>
    ServiceResolvers<RESOLVER...>::ServiceResolvers()
            : resolvers_({
                                 {RESOLVER::service_name_to_resolve_, &RESOLVER::instance()}...
                         })
    {/**/}


    template<typename... RESOLVER>
    optional_service_id_t
    ServiceResolvers<RESOLVER...>::service_name_to_id(const char* service_name, const char* req, size_t req_len) {
        //resolve service instance by service specific resolver
        auto iter = resolvers_.find(service_name);
        if (iter != resolvers_.end()) {
            return iter->second->service_name_to_id(service_name, req, req_len);
        }

        return boost::none;
    }

}

#endif //PROJECT_COMBINED_RESOLVER_H
