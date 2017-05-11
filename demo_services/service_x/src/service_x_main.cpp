#include <iostream>

#include <api/service_y/y_api_interface_declare.h>
#include <api/service_z/z_api_interface_declare.h>
#include <adapter_example/details/msgrpc_test.h>
#include <msgrpc/core/service_interaction/si_base.h>

#include <test_util/test_runner.h>
#include <msgrpc/core/cell/timeout_cell.h>

using namespace service_y;
using namespace service_z;
using namespace msgrpc;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//1. Top Level service resolver is the system default resolver in msgrpc::Config::instance().service_register_

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CommonServiceResolver : ServiceResolver {
    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        return boost::none;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<const char* SERVICE_NAME>
struct SingleServiceResolver : ServiceResolver {
    static const char* service_name_to_resolve_;
};

template<const char* SERVICE_NAME>
const char* SingleServiceResolver<SERVICE_NAME>::service_name_to_resolve_ = SERVICE_NAME;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MULTI_SERVICE_RESOLVER, typename... SINGLE_SERVICE_RESOLVER>
struct CombinedServiceResolver : ServiceResolver {
    typedef const char* service_name_t;
    std::map<service_name_t, ServiceResolver*> resolvers_;

    CombinedServiceResolver()
        : resolvers_({
                         {SINGLE_SERVICE_RESOLVER::service_name_to_resolve_, &SINGLE_SERVICE_RESOLVER::instance()}...
                     })
    { /**/ }

    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        return boost::none;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Y__ServiceResolver : SingleServiceResolver<service_y::g_service_name>, Singleton<Y__ServiceResolver> {
    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        return boost::none;
    }
};

struct Z__ServiceResolver : SingleServiceResolver<service_z::g_service_name>, Singleton<Z__ServiceResolver> {
    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        return boost::none;
    }
};

struct MyMultiServiceResolver : ServiceResolver, Singleton<MyMultiServiceResolver> {
    virtual optional_service_id_t service_name_to_id(const char* service_name, const char* req, size_t req_len) override {
        return boost::none;
    }
};


typedef CombinedServiceResolver<MyMultiServiceResolver, Y__ServiceResolver, Z__ServiceResolver>  MyServiceResolver;
MyServiceResolver resolver;


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

        run_next_testcase();
    };

    msg_loop_thread msg_loop_thread(x_service_id, x_init, not_drop_msg);

    return 0;
}
