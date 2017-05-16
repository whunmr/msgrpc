#ifndef PROJECT_RESOLVER_MACROS_H
#define PROJECT_RESOLVER_MACROS_H

#include <msgrpc/core/service_discovery/named_sr_listener.h>
#include <msgrpc/core/service_discovery/service_resolver.h>


#define DECLARE_SERVICE_RESOLVER(RESOLVER_NAME_, SERVICE_NAME_) \
    struct RESOLVER_NAME_ : msgrpc::NamedSRListener<SERVICE_NAME_>, msgrpc::NamedResolver<SERVICE_NAME_>, msgrpc::Singleton<RESOLVER_NAME_> {\
        RESOLVER_NAME_();\
        virtual msgrpc::optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override;\
        virtual void on_changes(const msgrpc::instance_vector_t& instances) override;\
        \
        static msgrpc::SRListener* instance_ptr;\
    };



#define DEFINE_AND_REGISTER_SERVICE_RESOLVER(RESOLVER_NAME_) \
    msgrpc::SRListener* RESOLVER_NAME_::instance_ptr = RESOLVER_NAME_::instance().register_as_listener(); \
    RESOLVER_NAME_::RESOLVER_NAME_() { \
        auto force_reference = instance_ptr; \
    }


#endif //PROJECT_RESOLVER_MACROS_H
