#ifndef PROJECT_ZK_SERVICE_REGISTER_H
#define PROJECT_ZK_SERVICE_REGISTER_H

#include <zookeeper/zookeeper.h>
#include <msgrpc/core/adapter/service_register.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#include <msgrpc/util/singleton.h>
#include <conservator/ConservatorFrameworkFactory.h>


namespace demo {
    void close_zk_connection_at_exit();

    struct ZkServiceRegister : msgrpc::ServiceRegister, msgrpc::Singleton<ZkServiceRegister> {
        const string service_root = "/services";

        bool is_zk_connected(const unique_ptr<ConservatorFramework> &zk) const {
            return zk && (zk->getState() == ZOO_CONNECTED_STATE);
        }

        bool assure_zk_is_connected() {
            if (is_zk_connected(zk_)) {
                return true;
            }

            if ( ! zk_) {
                std::atexit(close_zk_connection_at_exit);
            }

            //TODO: read zk server address from configuration
            auto zk = ConservatorFrameworkFactory().newClient("localhost:2181");
            zk->start();

            bool is_connected = is_zk_connected(zk);
            if (is_connected) {
                zk_ = std::move(zk);
            }

            return is_connected;
        }

        bool create_ephemeral_node_for_service_instance(const char* service_name, const char* version, const char *end_point) {
            int ret;
            ret = zk_->create()->forPath(service_root);
            ret = zk_->create()->forPath(service_root + "/" + service_name);

            const clientid_t* session_id = zoo_client_id(zk_->handle());
            if (session_id == nullptr) {
                return false;
            }

            string path = service_root + "/" + service_name + "/" + end_point + "@" + std::to_string(session_id->client_id) + "@" + version;
            ret = zk_->create()->withFlags(ZOO_EPHEMERAL)->forPath(path, end_point);

            bool result = (ZOK == ret || ZNODEEXISTS == ret);
            if (!result) {
                cout << "register service on zk failed: zk_reuslt: " << ret << " for path:" << path << endl;
            }

            cout << "result_path: " << path << endl;
            return result;
        }

        boost::optional<msgrpc::service_id_t> str_to_service_id(const string& endpoint) {
            size_t sep = endpoint.find(":");
            if (sep == string::npos) {
                return boost::none;
            }

            string ip = string(endpoint, 0, sep);
            unsigned short port = (unsigned short)strtoul(endpoint.c_str() + sep + 1, NULL, 0);

            return msgrpc::service_id_t(boost::asio::ip::address::from_string(ip), port);
        }

        //TODO: virtual bool init() override {
        ////fetch server list for first time and start watch /services node
        //}

        virtual bool register_service(const char* service_name, const char* version, const char *end_point) override {
            if (service_name == nullptr || end_point == nullptr) {
                return false;
            }

            if ( ! assure_zk_is_connected()) {
                std::cout << "[ERROR] connection to zk failed" << std::endl;
                return false;
            }

            return create_ephemeral_node_for_service_instance(service_name, version, end_point);
        }

        

        virtual boost::optional<msgrpc::service_id_t> service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
            if ( ! assure_zk_is_connected()) {
                std::cout << "[ERROR] connection to zk failed" << std::endl;
                return boost::none;
            }

            vector<string> children = zk_->getChildren()->forPath(service_root + "/" + service_name);
            for (auto child : children) {
                std::cout << "service instance: " << child << std::endl;
            }

            //TODO: save fetched service list into local service_discovery cache
            //TODO: first add auto register msg handler then schedule msg to self's msg queue.
            //TODO: 1. read services_cache_ and answer request of service_name_to_id directly
            //TODO: 2. populate services_cache_ at process init, and during handling of zookeeper watcher notifications.



            if (children.empty()) {
                return boost::none;
            }

            //TODO: select which service to route
            return str_to_service_id(children[0]);
        }

        ////////////////////////////////////////////////////////////////////////
        struct cmp_str {
            bool operator()(char const *a, char const *b) {
                return std::strcmp(a, b) < 0;
            }
        };

        typedef std::vector<msgrpc::service_id_t> service_vector_t;
        std::map<char *, service_vector_t, cmp_str> services_cache_;

        ////////////////////////////////////////////////////////////////////////
        unique_ptr<ConservatorFramework> zk_;
    };

    void close_zk_connection_at_exit() {
        if (ZkServiceRegister::instance().zk_)
            ZkServiceRegister::instance().zk_->close();
    }
}

#endif //PROJECT_ZK_SERVICE_REGISTER_H
