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
#include <msgrpc/core/service_discovery/service_filter.h>

namespace demo {

    typedef ConservatorFramework ZKHandle;
    typedef std::map<std::string, msgrpc::instance_vector_t> services_cache_t;

    void close_zk_connection_at_exit();


    struct ZkServiceRegister : msgrpc::ServiceRegister, msgrpc::Singleton<ZkServiceRegister> {

        virtual bool init() override;

        virtual bool register_service(const char* service_name, const char* version, const char *end_point) override;
        virtual msgrpc::optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override;

        virtual void register_listener(msgrpc::SRListener& listener) override;
        virtual void unregister_listener(msgrpc::SRListener& ___l) override;

        unique_ptr<ZKHandle> zk_;

      private:

        bool try_connect_zk();
        void wait_util_zk_is_connected();

        //TODO: peroidically test existence of ephemeral node of service, and create the ephemeral node if need.
        //      should re-register into zk, if zk's data was accidentally deleted.

        //TODO: should periodically fetch services/instances info from zk,
        //      incase miss notifications between handling watch notification and set watch again.

        bool create_ephemeral_node_for_service_instance(const char* service_name, const char* version, const char *end_point);

        static void session_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt);
        static void service_child_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt);
        static void instance_child_watcher_fn(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctxt);
        static void fetch_and_update_instances(ZkServiceRegister* srv_register, const string &service_name);

        vector<string> try_fetch_services();
        void do_notify_listeners(const string& service_name, const msgrpc::instance_vector_t& ___iv);
        bool fetch_service_instances_from_zk(const string& service, msgrpc::instance_vector_t& instances);
        bool try_fetch_services_from_zk(services_cache_t& cache, vector<string>& services);

      private:
        std::map<string, std::set<msgrpc::SRListener*>> listeners_map_;

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
    };
}

#endif //PROJECT_ZK_SERVICE_REGISTER_H
