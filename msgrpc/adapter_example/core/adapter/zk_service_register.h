#ifndef PROJECT_ZK_SERVICE_REGISTER_H
#define PROJECT_ZK_SERVICE_REGISTER_H

#include <msgrpc/util/singleton.h>
#include <msgrpc/core/adapter/service_register.h>
#include <iostream>

namespace demo {
    struct ZkServiceRegister : msgrpc::ServiceRegister, msgrpc::Singleton<ZkServiceRegister> {
        virtual void register_service(const char *end_point) {
            std::cout << "register by ZkServiceRegister: " << end_point << std::endl;
        }
    };
}

#endif //PROJECT_ZK_SERVICE_REGISTER_H
