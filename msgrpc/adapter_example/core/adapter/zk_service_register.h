#ifndef PROJECT_ZK_SERVICE_REGISTER_H
#define PROJECT_ZK_SERVICE_REGISTER_H

#include <msgrpc/core/adapter/service_register.h>
#include <cstdlib>
#include <iostream>
#include <msgrpc/util/singleton.h>
#include <conservator/ConservatorFrameworkFactory.h>

namespace demo {

    void close_zk_connection();

    struct ZkServiceRegister : msgrpc::ServiceRegister, msgrpc::Singleton<ZkServiceRegister> {

        virtual bool register_service(const char *end_point) override {
            ConservatorFrameworkFactory factory = ConservatorFrameworkFactory();

            unique_ptr<ConservatorFramework> framework = factory.newClient("localhost:2181");
            framework_ = std::move(framework);
            framework_->start();

            std::cout << "framework_->isStarted(): " << framework_->isStarted() << std::endl;
            std::cout << "(framework_->getState() ==  ZOO_CONNECTED_STATE) ? " << (framework_->getState() == ZOO_CONNECTED_STATE) << std::endl;

            std::atexit(close_zk_connection);

                        

            return true;
        }

        unique_ptr<ConservatorFramework> framework_;
    };

    void close_zk_connection() {
        ZkServiceRegister::instance().framework_->close();
    }
}

#endif //PROJECT_ZK_SERVICE_REGISTER_H
