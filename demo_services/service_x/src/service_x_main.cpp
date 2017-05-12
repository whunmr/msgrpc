#include <iostream>

#include <api/service_y/y_api_interface_declare.h>
#include <api/service_z/z_api_interface_declare.h>
#include <adapter_example/details/msgrpc_test.h>
#include <msgrpc/core/service_interaction/si_base.h>

#include <test_util/test_runner.h>
#include <msgrpc/core/cell/timeout_cell.h>
#include <msgrpc/util/instances_collector.h>

using namespace service_y;
using namespace service_z;
using namespace msgrpc;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//last service resolver is the system default resolver in msgrpc::Config::instance().service_register_
struct DefaultServiceResolver : ServiceResolver, Singleton<DefaultServiceResolver> {
    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        return msgrpc::Config::instance().service_register_->service_name_to_id(service_name, req, req_len);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<const char* SERVICE_NAME>
struct SResolverT : ServiceResolver {
    static const char* service_name_to_resolve_;
};

template<const char* SERVICE_NAME>
const char* SResolverT<SERVICE_NAME>::service_name_to_resolve_ = SERVICE_NAME;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename DEFAULT_RESOLVER, typename... RESOLVER>
struct CombinedServiceResolver
        : ServiceResolver
        , Singleton<CombinedServiceResolver<DEFAULT_RESOLVER, RESOLVER...>> {

    std::map<std::string, ServiceResolver*> resolvers_;

    CombinedServiceResolver()
        : resolvers_({
            {RESOLVER::service_name_to_resolve_, &RESOLVER::instance()}...
        })
    { /**/ }

    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        //1. resolve service instance by service specific resolver
        auto iter = resolvers_.find(service_name);
        if (iter != resolvers_.end()) {
            return iter->second->service_name_to_id(service_name, req, req_len);
        }

        //2. using default resolver, if can not find service specific resolver
        return DEFAULT_RESOLVER::instance().service_name_to_id(service_name, req, req_len);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<const char* SERVICE_NAME>
struct SRListenerT : ServiceRegisterListener {
    SRListenerT() {
        bool has_service_register = msgrpc::Config::instance().service_register_ != nullptr;

        if (has_service_register) {
            msgrpc::Config::instance().service_register_->register_listener(*this);
        } else {
            msgrpc::InstancesCollector<ServiceRegisterListener>::instance().track(*this);
        }
    }

    virtual const char* service_to_listener() override {
        return SERVICE_NAME;
    }
};

struct Y__ServiceResolver : SRListenerT<service_y::k_name>, SResolverT<service_y::k_name>, Singleton<Y__ServiceResolver> {
    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        ___log_debug("service_name_to_id from Y__ServiceResolver");
        return boost::none;
    }

    virtual void on_changes(const instance_vector_t& instances) override {
        ___log_debug("Y__ServiceResolver::on_changes");
    }
};

struct Z__ServiceResolver : SRListenerT<service_z::k_name>, SResolverT<service_z::k_name>, Singleton<Z__ServiceResolver> {
    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        ___log_debug("service_name_to_id from Z__ServiceResolver");
        return boost::none;
    }

    virtual void on_changes(const instance_vector_t& instances) override {
        ___log_debug("Z__ServiceResolver::on_changes");
    }
};

struct MyMultiServiceResolver : ServiceResolver, Singleton<MyMultiServiceResolver> {
    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        ___log_debug("service_name_to_id from MyMultiServiceResolver");
        return DefaultServiceResolver::instance().service_name_to_id(service_name, req, req_len);
    }

    //TODO: how to track changes of all services
};

//TODO: register a global service resolver into msgrpc::Config
typedef CombinedServiceResolver<MyMultiServiceResolver, Y__ServiceResolver, Z__ServiceResolver> MyServiceResolver;

void run_test_foo() {
    MyServiceResolver& resolver = MyServiceResolver::instance();
    resolver.service_name_to_id(service_y::k_name, nullptr, 0);
    resolver.service_name_to_id(service_z::k_name, nullptr, 0);
    resolver.service_name_to_id("service_z", nullptr, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEFINE_SI(SI_call_y_f1m1, YReq, YRsp) {
    auto call_y_f1m1 = [&ctxt, req]() {
        return IY(ctxt).___f1m1(req);
    };

    return ___rpc(___ms(10), call_y_f1m1);
}

DEF_TESTCASE(testcase_0000) {
    YReq yreq;
    yreq.yreqa = 100;

    auto* rsp_cell = SI_call_y_f1m1().run(yreq);

    derive_final_action([](msgrpc::Cell<YRsp>& r) {
        EXPECT_EQ(true, r.has_value());
        if (r.has_value())
            EXPECT_EQ(200, r.value().yrspa);

        run_next_testcase();
    }, rsp_cell);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEFINE_SI(SI_call_z_f1m1, ZReq, ZRsp) {
    auto call_y_f1m1 = [&ctxt, req]() {
        return IZ(ctxt).___z_f1m1(req);
    };

    return ___rpc(___ms(10), call_y_f1m1);
}

DEF_TESTCASE(testcase_0001) {
    ZReq zreq;
    zreq.zreqa = 100;

    auto* rsp_cell = SI_call_z_f1m1().run(zreq);

    derive_final_action([](msgrpc::Cell<ZRsp>& r) {
        EXPECT_EQ(true, r.has_value());
        if (r.has_value())
            EXPECT_EQ(7, r.value().zrspa);
        run_next_testcase();
    }, rsp_cell);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEFINE_SI(SI_call_y_async_f1m2, YReq, YRsp) {
    auto call_async_f1m2 = [&ctxt, req]() {
        return IY(ctxt).____async_f1m2(req);
    };

    return ___rpc(___ms(10000), call_async_f1m2);
}

DEF_TESTCASE(testcase_0002) {
    YReq yreq;
    yreq.yreqa = 100;

    auto* rsp_cell = SI_call_y_async_f1m2().run(yreq);

    derive_final_action([](msgrpc::Cell<YRsp>& r) {
        EXPECT_EQ(true, r.has_value());
        if (r.has_value())
            EXPECT_EQ(14, r.value().yrspa);
        run_next_testcase();
    }, rsp_cell);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    unsigned short port = 6666;
    const msgrpc::service_id_t x_service_id(boost::asio::ip::address::from_string("127.0.0.1"), port);

    std::cout << "[service_start_up] service_x_main" << std::endl;

    auto x_init = [port]{
        msgrpc::Config::instance().service_register_->init();

        run_test_foo();

        run_next_testcase();
    };

    msg_loop_thread msg_loop_thread(x_service_id, x_init, not_drop_msg);

    return 0;
}
