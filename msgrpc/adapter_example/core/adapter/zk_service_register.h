#ifndef PROJECT_ZK_SERVICE_REGISTER_H
#define PROJECT_ZK_SERVICE_REGISTER_H

#include <zookeeper/zookeeper.h>
#include <msgrpc/core/adapter/service_register.h>
#include <msgrpc/core/schedule/task_run_on_main_queue.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#include <msgrpc/util/singleton.h>
#include <conservator/ConservatorFrameworkFactory.h>

//TODO: split into .h and .cpp
namespace demo {
    void close_zk_connection_at_exit();

    const string k_services_root = "/services";

    /////////////////////////////////////////////////////////////////////
    struct InstanceInfo {
        msgrpc::service_id_t service_id_;
    };

    typedef std::vector<InstanceInfo> instance_vector_t;
    typedef ConservatorFramework ZKHandle;
    typedef std::map<string, instance_vector_t> services_cache_t;

    /////////////////////////////////////////////////////////////////////
    namespace {
        boost::optional<msgrpc::service_id_t> str_to_service_id(const string& endpoint) {
            size_t sep = endpoint.find(":");
            if (sep == string::npos) {
                return boost::none;
            }

            string ip = string(endpoint, 0, sep);
            unsigned short port = (unsigned short)strtoul(endpoint.c_str() + sep + 1, NULL, 0);

            return msgrpc::service_id_t(boost::asio::ip::address::from_string(ip), port);
        }


        void strings_to_instances(const vector<string>& instance_strings, instance_vector_t& instances) {
            //TODO: remove duplicated
            for (auto& si : instance_strings) {
                boost::optional<msgrpc::service_id_t> service_id = str_to_service_id(si);

                if (service_id) {
                    InstanceInfo ii;
                    ii.service_id_ = service_id.value();

                    instances.push_back(ii);
                }
            }
        }
    }

    struct ZkServiceRegister : msgrpc::ServiceRegister, msgrpc::Singleton<ZkServiceRegister> {
        bool is_zk_connected(const unique_ptr<ZKHandle> &zk) const {
            return zk && (zk->getState() == ZOO_CONNECTED_STATE);
        }

        static void session_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt) {
            if (type == ZOO_SESSION_EVENT) {
                cout << "got ZOO_SESSION_EVENT" << endl;
                //TODO: handle loss connection,  set state to loss_connection
                //TODO: handle reconnected to zk event, and reset watcher on services/instances znodes.
                return;
            }
        }

        bool try_connect_zk() {
            if (is_zk_connected(zk_)) {
                return true;
            }

            if ( ! zk_) {
                std::atexit(close_zk_connection_at_exit);
            }

            //TODO: read zk server address from configuration
            auto zk = ConservatorFrameworkFactory().newClient("localhost:2181");
            try {
                zk->start();
            } catch (const char* msg) {
                zk->close();
                cout << "[ERROR] catched exception: " << msg << endl;
                return false;
            }

            if ( ! is_zk_connected(zk)) {
                zk->close();
                return false;
            }

            zk->checkExists()->withWatcher(session_watcher_fn, this)->forPath("/");
            zk_ = std::move(zk);
            return true;
        }

        void wait_util_zk_is_connected() {
            bool connected;

            do {
                if ( ! (connected = try_connect_zk())) {
                    std::cout << "[ERROR] connection to zk failed" << std::endl;
                }
            } while( ! connected);
        }

        //TODO: peroidically test existence of ephemeral node of service, and create the ephemeral node if need.
        //      after macbook sleep, and after sleep the ephemeral nodes were disappeared.
        //      should re-register into zk, if zk's data was accidentally deleted.

        //TODO: should periodically fetch services/instances info from zk,
        //      incase miss notifications between handling watch notification and set watch again.

        //TODO: when connected to zk, and restart zk server, then we should able to reconnect to zk,
        //      instead of continuous print out "ZOO_ERROR@handle_socket_error_msg@1746: \
        //                                       Socket [::1:2181] zk retcode=-4, errno=64(Host is down): \
        //                                       failed while receiving a server response"
        //      test shutdown zk, wait 30 minutes, and restart zk, see if we'll connected again.


        bool create_ephemeral_node_for_service_instance(const char* service_name, const char* version, const char *end_point) {
            int ret;
            ret = zk_->create()->forPath(k_services_root);
            ret = zk_->create()->forPath(k_services_root + "/" + service_name);

            const clientid_t* session_id = zoo_client_id(zk_->handle());
            if (session_id == nullptr) {
                return false;
            }

            string path = k_services_root + "/" + service_name + "/" + end_point + "@" + std::to_string(session_id->client_id) + "@" + version;
            ret = zk_->create()->withFlags(ZOO_EPHEMERAL)->forPath(path, end_point);

            bool result = (ZOK == ret || ZNODEEXISTS == ret);
            if (!result) {
                cout << "register service on zk failed: zk_reuslt: " << ret << " for path:" << path << endl;
            }

            return result;
        }

        static void service_child_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt) {
            if (type != ZOO_CHILD_EVENT) {
                return;
            }

            ZkServiceRegister* srv_register = (ZkServiceRegister*)watcher_ctxt;

            services_cache_t cache;
            bool fetch_ok = srv_register->try_fetch_services_from_zk(cache);

            if (fetch_ok) {
                msgrpc::Task::dispatch_async_to_main_queue(
                    [srv_register, cache] {
                        srv_register->services_cache_ = cache;
                    }
                );
            }
        }

        static void instance_child_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt) {
            if (type != ZOO_CHILD_EVENT) {
                return;
            }

            assert(strlen(path) > k_services_root.length() + 1 /* / */  && "should only handle path starts with /services/");

            auto* srv_register = (ZkServiceRegister*)watcher_ctxt;
            string service_name = path + k_services_root.length() + 1 /* / */;

            instance_vector_t instances;
            bool fetch_ok = srv_register->fetch_service_instances_from_zk(service_name, instances);

            msgrpc::Task::dispatch_async_to_main_queue(
                [srv_register, service_name, instances] {
                    srv_register->services_cache_[service_name] = instances;
                }
            );
        }

        bool fetch_service_instances_from_zk(const string& service, instance_vector_t& instances) {
            bool connected = try_connect_zk();
            if (!connected) {
                cout << "[ERROR] try_fetch_services_from_zk failed, can not connect to zk." << endl;
                return false;
            }

            vector<string> instance_strings = zk_->getChildren()->withWatcher(instance_child_watcher_fn, this)->forPath(k_services_root + "/" + service);

            for (auto& service_instance : instance_strings) {
                std::cout << "[DEBUG]    instance list: " << service_instance << std::endl;
            }

            strings_to_instances(instance_strings, instances);
            return true;
        }

        bool try_fetch_services_from_zk(services_cache_t& cache) {
            bool connected = try_connect_zk();
            if (!connected) {
                cout << "[ERROR] try_fetch_services_from_zk failed, can not connect to zk." << endl;
                return false;
            }

            vector<string> services = zk_->getChildren()->withWatcher(service_child_watcher_fn, this)->forPath(k_services_root);

            for (auto& service : services) {
                std::cout << "[DEBUG] service list: " << service << std::endl;

                instance_vector_t instances;
                bool fetch_ok = fetch_service_instances_from_zk(service, instances);
                if (fetch_ok) {
                    cache[service] = instances;
                }
            }

            return true;
        }

        virtual bool init() override {
            wait_util_zk_is_connected();
            return try_fetch_services_from_zk(services_cache_);
        }

        virtual bool register_service(const char* service_name, const char* version, const char *end_point) override {
            if (service_name == nullptr || end_point == nullptr) {
                return false;
            }

            wait_util_zk_is_connected();

            return create_ephemeral_node_for_service_instance(service_name, version, end_point);
        }


        virtual boost::optional<msgrpc::service_id_t> service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
            //TODO: select which service to route
            const auto& iter = services_cache_.find(service_name);
            if (iter == services_cache_.end()) {
                return boost::none;
            }

            instance_vector_t& instances = iter->second;
            if (instances.empty()) {
                return boost::none;
            }

            return instances[0].service_id_;
        }

        ////////////////////////////////////////////////////////////////////////
        services_cache_t services_cache_;
        unique_ptr<ZKHandle> zk_;
    };

    void close_zk_connection_at_exit() {
        if (ZkServiceRegister::instance().zk_)
            ZkServiceRegister::instance().zk_->close();
    }
}

#endif //PROJECT_ZK_SERVICE_REGISTER_H
