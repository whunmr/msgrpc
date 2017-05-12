#ifndef PROJECT_NAMED_SR_LISTENER_H
#define PROJECT_NAMED_SR_LISTENER_H

#include <msgrpc/core/adapter/service_register.h>
#include <msgrpc/core/adapter/config.h>

namespace msgrpc {
    template<const char *SERVICE_NAME>
    struct NamedSRListener : SRListener {

        SRListener *register_as_listener() {
            bool has_service_register = msgrpc::Config::instance().service_register_ != nullptr;
            if (has_service_register) {
                msgrpc::Config::instance().service_register_->register_listener(*this);
            } else {
                msgrpc::InstancesCollector<SRListener>::instance().track(*this);
            }
            return this;
        }

        virtual const char *service_to_listener() override {
            return SERVICE_NAME;
        }
    };
}

#endif //PROJECT_NAMED_SR_LISTENER_H
