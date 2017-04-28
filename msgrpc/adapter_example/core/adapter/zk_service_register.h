#ifndef PROJECT_ZK_SERVICE_REGISTER_H
#define PROJECT_ZK_SERVICE_REGISTER_H

#include <zookeeper/zookeeper.h>
#include <msgrpc/core/adapter/service_register.h>
#include <cstdlib>
#include <iostream>
#include <msgrpc/util/singleton.h>
#include <conservator/ConservatorFrameworkFactory.h>

namespace demo {

    void close_zk_connection_at_exit();

    struct ZkServiceRegister : msgrpc::ServiceRegister, msgrpc::Singleton<ZkServiceRegister> {

        virtual bool register_service(const char* service_name, const char *end_point) override {
            if (service_name == nullptr || end_point == nullptr) {
                return false;
            }

            auto framework = ConservatorFrameworkFactory().newClient("localhost:2181");
            framework_ = std::move(framework);
            framework_->start();

            std::cout << "framework_->isStarted(): " << framework_->isStarted() << std::endl;
            std::cout << "(framework_->getState() ==  ZOO_CONNECTED_STATE) ? " << (framework_->getState() == ZOO_CONNECTED_STATE) << std::endl;

            std::atexit(close_zk_connection_at_exit);

            string services = "/services";

            int ret;
            ret = framework_->create()->forPath(services);
            ret = framework_->create()->forPath(services + "/" + service_name);
            ret = framework_->create()->withFlags(ZOO_EPHEMERAL | ZOO_SEQUENCE)->forPath(services + "/" + service_name + "/instance-");

            cout << "register result ZOK == ret: " << (ZOK == ret) << endl;
            return ZOK == ret;
        }

        unique_ptr<ConservatorFramework> framework_;
    };

    void close_zk_connection_at_exit() {
        ZkServiceRegister::instance().framework_->close();
    }
}

#endif //PROJECT_ZK_SERVICE_REGISTER_H
