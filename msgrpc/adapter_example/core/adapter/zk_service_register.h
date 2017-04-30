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

        bool zk_is_connected(const unique_ptr<ConservatorFramework>& zk) const {
            return zk && (zk->getState() == ZOO_CONNECTED_STATE);
        }

        bool assure_zk_is_connected() {
            if (zk_is_connected(zk_)) {
                return true;
            }

            if ( ! zk_) {
                std::atexit(close_zk_connection_at_exit);
            }

            //TODO: read zk server address from configuration
            auto zk = ConservatorFrameworkFactory().newClient("localhost:2181");
            zk->start();

            bool is_connected = zk_is_connected(zk);
            if (is_connected) {
                zk_ = std::move(zk);
            }

            std::cout << "(zk_->getState() ==  ZOO_CONNECTED_STATE) ? " << is_connected << std::endl;

            return is_connected;
        }

        virtual bool register_service(const char* service_name, const char *end_point) override {
            if (service_name == nullptr || end_point == nullptr) {
                return false;
            }

            if ( ! assure_zk_is_connected()) {
                std::cout << "[ERROR] connection to zk failed" << std::endl;
                return false;
            }

            string services = "/services";

            int ret;
            ret = zk_->create()->forPath(services);
            ret = zk_->create()->forPath(services + "/" + service_name);

            ret = zk_->create()->withFlags(ZOO_EPHEMERAL | ZOO_SEQUENCE)->forPath(services + "/" + service_name + "/instance-", end_point, ephemeral_node_path_);

            cout << "register result ZOK == ret: " << (ZOK == ret) << endl;
            cout << "result_path: " << ephemeral_node_path_ << endl;

            return ZOK == ret;
        }

        virtual msgrpc::service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
            unsigned short port = (strcmp(service_name, "service_x") == 0) ? 6666 /*x_service_id*/ : 7777 /*y_service_id*/;
            return msgrpc::service_id_t(boost::asio::ip::address::from_string("127.0.0.1"), port);
        }

        unique_ptr<ConservatorFramework> zk_;
        string ephemeral_node_path_;
    };

    void close_zk_connection_at_exit() {
        if (ZkServiceRegister::instance().zk_)
            ZkServiceRegister::instance().zk_->close();
    }
}

#endif //PROJECT_ZK_SERVICE_REGISTER_H
