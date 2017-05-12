#ifndef PROJECT_ZK_SERVICE_REGISTER_H
#define PROJECT_ZK_SERVICE_REGISTER_H

#include <zookeeper/zookeeper.h>
#include <msgrpc/core/adapter/service_register.h>
#include <msgrpc/core/schedule/task_run_on_main_queue.h>
#include <msgrpc/core/adapter/logger.h>
#include <cstdlib>
#include <map>

#include <msgrpc/util/singleton.h>
#include <msgrpc/util/instances_collector.h>
#include <conservator/ConservatorFrameworkFactory.h>

//TODO: split into .h and .cpp
namespace demo {
    void close_zk_connection_at_exit();

    const string k_services_root = "/services";

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    using msgrpc::InstanceInfo;
    using msgrpc::instance_vector_t;
    using msgrpc::ServiceRegisterListener;

    typedef ConservatorFramework ZKHandle;
    typedef std::map<std::string, instance_vector_t> services_cache_t;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct instance_info_compare {
        bool operator() (const InstanceInfo &a, const InstanceInfo &b) const { return a.service_id_ < b.service_id_; }
    };

    typedef std::set<InstanceInfo, instance_info_compare> instance_set_t;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
            instance_set_t instance_set;

            for (const auto& si : instance_strings) {
                boost::optional<msgrpc::service_id_t> service_id = str_to_service_id(si);

                if (service_id) {
                    InstanceInfo ii;
                    ii.service_id_ = service_id.value();

                    instance_set.insert(ii);
                }
            }

            instances.assign(instance_set.begin(), instance_set.end());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ZkServiceRegister : msgrpc::ServiceRegister, msgrpc::Singleton<ZkServiceRegister> {
        bool is_zk_connected(const unique_ptr<ZKHandle> &zk) const {
            return zk && (zk->getState() == ZOO_CONNECTED_STATE);
        }

        static void session_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt) {
            if (type == ZOO_SESSION_EVENT) {

                if (ZOO_CONNECTING_STATE == state) {
                    ___log_warning("zookeeper connection status changed to: ZOO_CONNECTING_STATE");
                } else if (ZOO_CONNECTED_STATE == state) {
                    ___log_warning("zookeeper connection status changed to: ZOO_CONNECTED_STATE");
                } else {
                    ___log_warning("zookeeper connection status changed to: %d", state);
                }
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
                ___log_error("catched exception during zk_start: %s", msg);
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
                    ___log_warning("connect to zookeeper failed, will continue retry.");
                }
            } while( ! connected);
        }

        //TODO: peroidically test existence of ephemeral node of service, and create the ephemeral node if need.
        //      should re-register into zk, if zk's data was accidentally deleted.

        //TODO: should periodically fetch services/instances info from zk,
        //      incase miss notifications between handling watch notification and set watch again.

        bool create_ephemeral_node_for_service_instance(const char* service_name, const char* version, const char *end_point) {
            int ret;
            ret = zk_->create()->forPath(k_services_root);
                  zk_->getChildren()->withWatcher(service_child_watcher_fn, this)->forPath(k_services_root);

            ret = zk_->create()->forPath(k_services_root + "/" + service_name);

            const clientid_t* session_id = zoo_client_id(zk_->handle());
            if (session_id == nullptr) {
                ___log_error("can not get session id of zookeeper client.");
                return false;
            }

            string path = k_services_root + "/" + service_name + "/" + end_point + "@" + std::to_string(session_id->client_id) + "@" + version;
            ret = zk_->create()->withFlags(ZOO_EPHEMERAL)->forPath(path, end_point);

            bool result = (ZOK == ret || ZNODEEXISTS == ret);
            if (!result) {
                ___log_error("register service on zk failed, path: %s, zk_reuslt: %d", path.c_str(), ret);
            }

            return result;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void service_child_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt) {
            if (type != ZOO_CHILD_EVENT) {
                return;
            }

            auto* srv_register = (ZkServiceRegister*)watcher_ctxt;

            vector<string> services = srv_register->try_fetch_services();
            {
                vector<string> changed_services;
                {
                    std::sort(services.begin(), services.end());
                    set_difference(services.begin(), services.end(), old_services_.begin(), old_services_.end(), back_inserter(changed_services) );
                    set_difference(old_services_.begin(), old_services_.end(), services.begin(), services.end(), back_inserter(changed_services) );
                }

                for (const auto& ___s : changed_services) {
                    fetch_and_update_instances(srv_register, ___s);
                }
            }

            old_services_ = services;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void instance_child_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt) {
            if (type != ZOO_CHILD_EVENT) {
                return;
            }

            assert(strlen(path) > k_services_root.length() + 1 /* / */  && "expects path starts with /services/");
            string service_name = path + k_services_root.length() + 1 /* / */;

            fetch_and_update_instances((ZkServiceRegister*)watcher_ctxt, service_name);
        }

        static void fetch_and_update_instances(ZkServiceRegister* srv_register, const string &service_name) {
            instance_vector_t ___iv;

            bool fetch_ok = srv_register->fetch_service_instances_from_zk(service_name, ___iv);

            if (fetch_ok) {
                msgrpc::Task::dispatch_async_to_main_queue(
                    [srv_register, service_name, ___iv] {
                        srv_register->services_cache_[service_name] = ___iv;

                        srv_register->do_notify_listeners(service_name, ___iv);
                    }
                );
            }
        }

        void do_notify_listeners(const string& service_name, const instance_vector_t& ___iv) {
            auto iter = listeners_map_.find(service_name);
            if (iter != listeners_map_.end()) {
                for (auto &___l : iter->second) {
                    ___l->on_changes(___iv);
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        vector<string> try_fetch_services() {
            return zk_->getChildren()->withWatcher(service_child_watcher_fn, this)->forPath(k_services_root);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool fetch_service_instances_from_zk(const string& service, instance_vector_t& instances) {
            bool connected = try_connect_zk();
            if (!connected) {
                ___log_error("fetch_service_instances_from_zk failed, can not connect to zk.");
                return false;
            }

            string service_path = k_services_root + "/" + service;
            vector<string> instance_strings = zk_->getChildren()->withWatcher(instance_child_watcher_fn, this)->forPath(service_path);

            for (const auto& service_instance : instance_strings) {
                ___log_debug("    %s instance : %s", service.c_str(), service_instance.c_str());
            }

            strings_to_instances(instance_strings, instances);
            return true;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool try_fetch_services_from_zk(services_cache_t& cache, vector<string>& services) {
            bool connected = try_connect_zk();
            if (!connected) {
                ___log_error("try_fetch_services_from_zk failed, can not connect to zk.");
                return false;
            }

            vector<string> latest_services = zk_->getChildren()->withWatcher(service_child_watcher_fn, this)->forPath(k_services_root);

            services = latest_services;
            std::sort(services.begin(), services.end());

            for (const auto& service : latest_services) {
                instance_vector_t ___iv;

                bool fetch_ok = fetch_service_instances_from_zk(service, ___iv);
                if (fetch_ok) {
                    ___log_debug("fetched service list for service: %s, with %d listener(s)", service.c_str(), listeners_map_.size());

                    cache[service] = ___iv;
                    do_notify_listeners(service, ___iv);
                }
            }

            return true;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool init() override {
            auto listeners = msgrpc::InstancesCollector<ServiceRegisterListener>::instance().instances();
            for (auto& listener : listeners) {
                this->register_listener(*listener);
            }

            wait_util_zk_is_connected();
            return try_fetch_services_from_zk(services_cache_, old_services_);
        }

        virtual bool register_service(const char* service_name, const char* version, const char *end_point) override {
            if (service_name == nullptr || end_point == nullptr) {
                return false;
            }

            wait_util_zk_is_connected();

            return create_ephemeral_node_for_service_instance(service_name, version, end_point);
        }

        virtual msgrpc::optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
            const auto& iter = services_cache_.find(service_name);
            if (iter == services_cache_.end()) {
                return boost::none;
            }

            instance_vector_t& instances = iter->second;
            if (instances.empty()) {
                return boost::none;
            }

            size_t size = instances.size();
            if (size == 1) {
                return instances[0].service_id_;
            }

            //using round-robin as default load-balance strategy
            size_t next_rr_index = round_robin_map_[service_name];
            round_robin_map_[service_name] = next_rr_index + 1;

            return instances[ next_rr_index % size ].service_id_;
        }


        virtual void register_listener(ServiceRegisterListener& listener) override {
            std::set<ServiceRegisterListener*>& listeners = listeners_map_[listener.service_to_listener()];
            listeners.insert(&listener);

            string service_name = listener.service_to_listener();

            auto iter = services_cache_.find(service_name);
            if (iter != services_cache_.end()) {
                this->do_notify_listeners(service_name, iter->second);
            }
        }

        virtual void unregister_listener(ServiceRegisterListener& ___l) override {
            auto iter = listeners_map_.find(___l.service_to_listener());
            if (iter == listeners_map_.end()) {
                return;
            }

            std::set<ServiceRegisterListener*>& listeners = iter->second;
            listeners.erase(&___l);
        }

        std::map<string, std::set<ServiceRegisterListener*>> listeners_map_;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::map<string, size_t> round_robin_map_;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //1. the zk client can only access this services_cache_ during init().
        //2. otherwise, all access to services_cache_ should schedule as task and dispatch into main_queue. like:
        //    msgrpc::Task::dispatch_async_to_main_queue(
        //        [srv_register, cache] {
        //            srv_register->services_cache_ = cache;
        //        }
        //    );
        services_cache_t services_cache_;      //owned by main queue thread

        static vector<string> old_services_;   //owned by zk client thread

        unique_ptr<ZKHandle> zk_;
    };

    void close_zk_connection_at_exit() {
        if (ZkServiceRegister::instance().zk_)
            ZkServiceRegister::instance().zk_->close();
    }
}

#endif //PROJECT_ZK_SERVICE_REGISTER_H
