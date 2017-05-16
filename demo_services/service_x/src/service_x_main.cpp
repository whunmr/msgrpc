#include <api/service_y/y_api_interface_declare.h>
#include <api/service_z/z_api_interface_declare.h>
#include <adapter_example/details/msgrpc_test.h>
#include <msgrpc/core/service_interaction/si_base.h>

#include <test_util/test_runner.h>
#include <msgrpc/core/cell/timeout_cell.h>

#include <msgrpc/core/service_discovery/default_service_resolver.h>
#include <service_resolvers/service_y_resolver.h>
#include <service_resolvers/service_z_resolver.h>
#include <msgrpc/core/service_discovery/service_resolvers.h>

using namespace service_y;
using namespace service_z;
using namespace msgrpc;

typedef ServiceResolvers<DefaultServiceResolver, Y__ServiceResolver, Z__ServiceResolver> MyServiceResolver;

void run_test_foo() {
    MyServiceResolver& resolver = MyServiceResolver::instance();
    resolver.service_name_to_id(service_y::k_name, nullptr, 0);
    resolver.service_name_to_id(service_z::k_name, nullptr, 0);
    resolver.service_name_to_id("service_y", nullptr, 0);
    resolver.service_name_to_id("service_foo", nullptr, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEFINE_SI_WITH_RESOLVER(SI_call_y_f1m1, YReq, YRsp, MyServiceResolver) {
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

    auto x_init = [port]{
        ___log_debug("[service_start_up] service_x_main");

        msgrpc::Config::instance().service_register_->init();

        run_test_foo();

        run_next_testcase();
    };

    msg_loop_thread msg_loop_thread(x_service_id, x_init, not_drop_msg);

    return 0;
}
