#include <iostream>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <msgrpc/thrift_struct/thrift_codec.h>
#include <type_traits>
#include <future>
#include <atomic>

#include "demo/demo_api_declare.h"

#include <msgrpc/core/typedefs.h>
#include <msgrpc/core/adapter/timer_adapter.h>
#include <msgrpc/core/rpc_sequence_id.h>
#include <msgrpc/core/adapter/config.h>
#include <msgrpc/test/core/adapter/simple_timer_adapter.h>
#include <msgrpc/test/test_util/UdpChannel.h>
#include <msgrpc/core/cell/rsp_sink.h>
#include <msgrpc/core/components/rsp_dispatcher.h>
#include <msgrpc/core/cell/cell.h>
#include <msgrpc/core/cell/derived_cell.h>
#include <msgrpc/core/cell/timeout_cell.h>
#include <msgrpc/core/service_interaction/si_base.h>
#include <msgrpc/core/iface_impl/iface_impl_utility.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <msgrpc/test/core/adapter/udp_msg_channel.h>
#include <msgrpc/test/details/set_timer_handler.h>
#include <msgrpc/core/components/req_msg_handler.h>
#include <msgrpc/core/components/rpc_timeout_handler.h>
#include <msgrpc/util/singleton.h>
#include <msgrpc/core/iface_impl/iface_impl_base_t.h>
#include <msgrpc/core/iface_stub/iface_stub_base.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "TemplateArgumentsIssues"
using namespace demo;

//constants for testing.
const int k_req_init_value = 1;
const int k__sync_y__delta = 3;
const int k__sync_x__delta = 17;

////////////////////////////////////////////////////////////////////////////////
//TODO: define following macros:
#define declare_interface_stub
#define  define_interface_stub
#define declare_interface_impl
#define  define_interface_impl

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------generate by:  declare and define stub macros
struct InterfaceXStub : msgrpc::IfaceStubBase {
    using msgrpc::IfaceStubBase::IfaceStubBase;
    msgrpc::Cell<ResponseBar>* ______sync_x(const RequestFoo&);
};

msgrpc::Cell<ResponseBar>* InterfaceXStub::______sync_x(const RequestFoo& req) {
    return encode_request_and_send<RequestFoo, ResponseBar>(1, 1, req);
}

////////////////////////////////////////////////////////////////////////////////
//---------------- generate this part by macros set:
struct InterfaceXImpl : msgrpc::InterfaceImplBaseT<InterfaceXImpl, 1> {
    msgrpc::Cell<ResponseBar>* ______sync_x(const RequestFoo& req);

    virtual msgrpc::RpcResult onRpcInvoke(const msgrpc::ReqMsgHeader& msg_header
            , const char* msg, size_t len
            , msgrpc::RspMsgHeader& rsp_header
            , msgrpc::service_id_t& sender_id) override;
};

////////////////////////////////////////////////////////////////////////////////
//---------------- generate this part by macros set: interface_implement_define.h
InterfaceXImpl interfaceXImpl_auto_register_instance;
msgrpc::RpcResult InterfaceXImpl::onRpcInvoke( const msgrpc::ReqMsgHeader& req_header, const char* msg
        , size_t len, msgrpc::RspMsgHeader& rsp_header
        , msgrpc::service_id_t& sender_id) {

    msgrpc::RpcResult ret;

    if (req_header.method_index_in_interface_ == 1) {
        ret = this->invoke_templated_method(&InterfaceXImpl::______sync_x, msg, len, sender_id, rsp_header);
    } else

    {
        rsp_header.rpc_result_ = msgrpc::RpcResult::method_not_found;
        return msgrpc::RpcResult::failed;
    }

    if (ret == msgrpc::RpcResult::failed) {
        rsp_header.rpc_result_ = msgrpc::RpcResult::failed;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
msgrpc::Cell<ResponseBar>* InterfaceXImpl::______sync_x(const RequestFoo& req) {
    std::cout << "                     ______sync_x" << std::endl;

    return msgrpc::call_sync_iface_impl<ResponseBar>(
        [&req](ResponseBar &rsp) {
            rsp.__set_rspa(req.reqa + k__sync_x__delta);
        }
    );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------generate by:  declare and define stub macros
struct InterfaceYStub : msgrpc::IfaceStubBase {
    using msgrpc::IfaceStubBase::IfaceStubBase;

    msgrpc::Cell<ResponseBar>* ______sync_y(const RequestFoo&);
    msgrpc::Cell<ResponseBar>* _____async_y(const RequestFoo&);
    msgrpc::Cell<ResponseBar>* ______sync_y_failed(const RequestFoo&);
    msgrpc::Cell<ResponseBar>* ______sync_y_failed_immediately(const RequestFoo&);
};

msgrpc::Cell<ResponseBar>* InterfaceYStub::______sync_y(const RequestFoo& req) {
    return encode_request_and_send<RequestFoo, ResponseBar>(2, 1, req);       
}

msgrpc::Cell<ResponseBar>* InterfaceYStub::_____async_y(const RequestFoo& req) {
    return encode_request_and_send<RequestFoo, ResponseBar>(2, 2, req);
}

msgrpc::Cell<ResponseBar>* InterfaceYStub::______sync_y_failed(const RequestFoo& req) {
    return encode_request_and_send<RequestFoo, ResponseBar>(2, 3, req);
}

msgrpc::Cell<ResponseBar>* InterfaceYStub::______sync_y_failed_immediately(const RequestFoo& req) {
    return msgrpc::Cell<ResponseBar>::new_failed_instance();
}

////////////////////////////////////////////////////////////////////////////////
//---------------- generate this part by macros set:
struct InterfaceYImpl : msgrpc::InterfaceImplBaseT<InterfaceYImpl, 2> {
    msgrpc::Cell<ResponseBar>* ______sync_y(const RequestFoo& req);
    msgrpc::Cell<ResponseBar>* _____async_y(const RequestFoo& req);
    msgrpc::Cell<ResponseBar>* ______sync_y_failed(const RequestFoo& req);
    msgrpc::Cell<ResponseBar>* ______sync_y_failed_immediately(const RequestFoo&);

    virtual msgrpc::RpcResult onRpcInvoke( const msgrpc::ReqMsgHeader& msg_header
            , const char* msg, size_t len
            , msgrpc::RspMsgHeader& rsp_header
            , msgrpc::service_id_t& sender_id) override;
};

////////////////////////////////////////////////////////////////////////////////
//---------------- generate this part by macros set: interface_implement_define.h
InterfaceYImpl interfaceYImpl_auto_register_instance;
msgrpc::RpcResult InterfaceYImpl::onRpcInvoke( const msgrpc::ReqMsgHeader& req_header
                                             , const char* msg
                                             , size_t len
                                             , msgrpc::RspMsgHeader& rsp_header
                                             , msgrpc::service_id_t& sender_id) {
    msgrpc::RpcResult ret;

    if (req_header.method_index_in_interface_ == 1) {
        ret = this->invoke_templated_method(&InterfaceYImpl::______sync_y, msg, len, sender_id, rsp_header);
    } else

    if (req_header.method_index_in_interface_ == 2) {
        ret = this->invoke_templated_method(&InterfaceYImpl::_____async_y, msg, len, sender_id, rsp_header);
    } else

    if (req_header.method_index_in_interface_ == 3) {
        ret = this->invoke_templated_method(&InterfaceYImpl::______sync_y_failed, msg, len, sender_id, rsp_header);
    } else

    if (req_header.method_index_in_interface_ == 4) {
        ret = this->invoke_templated_method(&InterfaceYImpl::______sync_y_failed_immediately, msg, len, sender_id, rsp_header);
    } else

    {
        rsp_header.rpc_result_ = msgrpc::RpcResult::method_not_found;
        return msgrpc::RpcResult::method_not_found;
    }

    if (ret == msgrpc::RpcResult::failed) {
        rsp_header.rpc_result_ = msgrpc::RpcResult::failed;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
msgrpc::Cell<ResponseBar>* InterfaceYImpl::______sync_y(const RequestFoo& req) {
    std::cout << "                     ______sync_y" << std::endl;

    return msgrpc::call_sync_iface_impl<ResponseBar>(
            [&req](ResponseBar &rsp) {
                rsp.__set_rspa(req.reqa + k__sync_y__delta);
            }
    );
}

struct SI_____async_y : msgrpc::SIBase<RequestFoo, ResponseBar> {
    virtual msgrpc::Cell<ResponseBar>* do_run(const RequestFoo &req, msgrpc::RpcContext& ctxt) override {
        return InterfaceXStub(ctxt).______sync_x(req);
    }
};

msgrpc::Cell<ResponseBar>* InterfaceYImpl::_____async_y(const RequestFoo& req) {
    std::cout << "                     _____async_y" << std::endl;
    return SI_____async_y().run(req);
}

msgrpc::Cell<ResponseBar>* InterfaceYImpl::______sync_y_failed(const RequestFoo& req) {
    std::cout << "                     ______sync_y_failed" << std::endl;
    return Cell<ResponseBar>::new_failed_instance();
}

msgrpc::Cell<ResponseBar>* ______sync_y_failed_immediately(const RequestFoo&) {
    return Cell<ResponseBar>::new_failed_instance();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <include/msgrpc/core/cell/derived_action.h>
#include <msgrpc/test/details/msgrpc_test_loop.h>
#include <msgrpc/test/details/msgrpc_test.h>
using namespace msgrpc;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case000 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        return InterfaceYStub(ctxt).______sync_y(req);
    }
};

TEST_F(MsgRpcTest, rpc__should_able_to_support___SI_with_single_rpc___case000) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value());
        EXPECT_EQ(k_req_init_value + k__sync_y__delta, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case000>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case100 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        return InterfaceYStub(ctxt)._____async_y(req);
    }
};

TEST_F(MsgRpcTest, rpc__should_able_to_support___SI_with_single_rpc___case100) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value());
        EXPECT_EQ(k_req_init_value + k__sync_x__delta, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case100>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void save_rsp_from_other_services_to_db(Cell<ResponseBar>& r) { std::cout << "1/2 ----------------->>>> write db." << std::endl; };
void save_rsp_to_log(Cell<ResponseBar>& r)                    { std::cout << "2/2 ----------------->>>> save_log." << std::endl; };

struct SI_case200 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
                    ___action(save_rsp_from_other_services_to_db, ___1);
                    ___action(save_rsp_to_log, ___1);
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to_support___SI_with_single_rpc____which_bind_with_actions______________case200) {
    // x ----(req)---->y (sync_y)
    // x <---(components)-----y
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value());
        EXPECT_EQ(k_req_init_value + k__sync_y__delta, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&] {rpc_main<SI_case200>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void merge_logic(Cell<ResponseBar>& result, Cell<ResponseBar>& cell_1, Cell<ResponseBar>& cell_2)  {
    if (cell_1.has_value() && cell_2.has_value()) {
        ResponseBar bar;
        bar.rspa = cell_1.value().rspa + cell_2.value().rspa;
        result.set_value(bar);
    }
};

struct SI_case300 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___2 = InterfaceYStub(ctxt)._____async_y(req);
        return ___cell(merge_logic, ___1, ___2);
    }
};

TEST_F(MsgRpcTest, should_able_to_support__SI_with_concurrent_rpc__and__merge_multiple_rpc_result________case300) {
    // x ----(req1)-------------------------->y  (async_y)
    // x ----(req1)-------------------------->y  (async_y)
    //        x (sync_x) <=========(req2)=====y  (async_y)
    //        x (sync_x) <=========(req2)=====y  (async_y)
    //        x (sync_x) ==========(rsp2)====>y  (async_y)
    // x <---(rsp1)---------------------------y  (async_y)
    //        x (sync_x) ==========(rsp2)====>y  (async_y)
    // x <---(rsp1)---------------------------y  (async_y)
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value());
        int expect_value = (k_req_init_value + k__sync_x__delta) * 2;
        EXPECT_EQ(expect_value, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case300>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Cell<ResponseBar>* call__sync_y(RpcContext &ctxt, Cell<ResponseBar> &___r) {
    if (___r.has_error()) {
        return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, ___r.failed_reason());
    }

    RequestFoo req;
    req.reqa = ___r.value().rspa;
    return InterfaceYStub(ctxt).______sync_y(req); //TODO: let rpc request return reference to cell
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Cell<ResponseBar>* call__sync_y_failed(RpcContext &ctxt, Cell<ResponseBar> &___r) {
    if (___r.has_error()) {
        return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, ___r.failed_reason());
    }

    RequestFoo req;
    req.reqa = ___r.value().rspa;
    return InterfaceYStub(ctxt).______sync_y_failed(req);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case4001 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto init_first_rpc        = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };
        auto call_sync_y_after___1 = [&ctxt](Cell<ResponseBar>& ___r) { return call__sync_y(ctxt, ___r); };
        auto call_sync_y_after___2 = [&ctxt](Cell<ResponseBar>& ___r) { return call__sync_y(ctxt, ___r); };

        auto ___1 = ___rpc(___ms(10), init_first_rpc);
        auto ___2 = ___rpc(___ms(10), call_sync_y_after___1, ___1);
        auto ___3 = ___rpc(___ms(10), call_sync_y_after___2, ___2);

        return ___3;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__SI_with_sequential_rpc______case4001) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value());
        EXPECT_EQ(k_req_init_value + k__sync_y__delta * 3, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4001>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                   , not_drop_msg );
    test_thread thread_timer(timer_service_id, []{}                           , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case4011_failed : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto init_first_rpc        = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y_failed(req); };
        auto call_sync_y_after___1 = [&ctxt](Cell<ResponseBar>& ___r) { return call__sync_y(ctxt, ___r); };
        auto call_sync_y_after___2 = [&ctxt](Cell<ResponseBar>& ___r) { return call__sync_y(ctxt, ___r); };

        auto ___1 = ___rpc(___ms(10), init_first_rpc);
        auto ___2 = ___rpc(___ms(10), call_sync_y_after___1, ___1);
        auto ___3 = ___rpc(___ms(10), call_sync_y_after___2, ___2);

        return ___3;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__failure_propagation__during__middle_of_sequential_rpc______case4011) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value());
        EXPECT_EQ(RpcResult::failed, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4011_failed>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                          , not_drop_msg );
    test_thread thread_timer(timer_service_id, []{}                                  , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case4021_failed : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {

        auto init_first_rpc               = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };
        auto call_sync_y_failed_after___1 = [&ctxt](Cell<ResponseBar>& ___r) { return call__sync_y_failed(ctxt, ___r); };
        auto call_sync_y_again            = [&ctxt](Cell<ResponseBar>& ___r) { return call__sync_y(ctxt, ___r); };

        auto ___1 = ___rpc(___ms(10), init_first_rpc);
        auto ___2 = ___rpc(___ms(10), call_sync_y_failed_after___1, ___1);
        auto ___3 = ___rpc(___ms(10), call_sync_y_again, ___2);

        return ___3;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__failure_propagation__during__middle_of_sequential_rpc______case4021) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value());
        EXPECT_EQ(RpcResult::failed, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4021_failed>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                          , not_drop_msg );
    test_thread thread_timer(timer_service_id, []{}                                  , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gen2(Cell<ResponseBar> &result, Cell<ResponseBar> &rsp_cell_1)  {
    if (rsp_cell_1.has_value()) {
        result.set_value(rsp_cell_1);
    }
};

void action1(Cell<ResponseBar> &r) { std::cout << "1/1 ----------------->>>> action1." << std::endl; };

struct SI_case500 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___3 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___1 = InterfaceYStub(ctxt)._____async_y(req);
                    ___action(action1, ___1);

                    auto ___2 = ___cell(gen2, ___1);
                                return ___cell(merge_logic, ___2, ___3);
    }
};

TEST_F(MsgRpcTest, should_able_to_support___parallel_rpcs_merge_after___1_transform_into__cell___2________case500) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value());
        int expect_value = (k_req_init_value + k__sync_x__delta) * 2;
        EXPECT_EQ(expect_value, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case500>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gen6(Cell<ResponseBar> &result, Cell<ResponseBar> &rsp)  {
    return rsp.has_error() ? result.set_failed_reason(rsp.failed_reason())
                           : result.set_value(rsp);
};

struct SI_case600 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y_failed(req);
        return ___cell(gen6, ___1);
    }
};

TEST_F(MsgRpcTest, should_able_to_support_failure_propagation__during__bind_cell_____case600) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case600>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TODO: add service discovery
//TODO: handle concor case: when a detached rpc's cell are not in dependency graph of final result cell,
//      if the result cell finished, can not release response handler of the detached cell.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case700_timeout : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__rpc_with_timer_and_retry___case700) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value());
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case700_timeout>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                          , drop_all_msg);
    test_thread thread_timer(timer_service_id, []{}                                  , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void join_rollback_cells(Cell<ResponseBar> &result, Cell<ResponseBar> &___1, Cell<ResponseBar> &___2, Cell<ResponseBar> &___3)  {
    bool ___1_got_value = ___1.has_value();
    bool ___2_and___3__both_finished = ___2.has_value_or_error() && ___3.has_value_or_error();

    if (___1_got_value || ___2_and___3__both_finished) {
        result.set_cell_value(___1);
    }
};

void run_customized_action(CellBase<bool> &r) {
    //TODO: access execution context, e.g.: access user id, link id, etc.
    //TODO: store some intermedia data in context
    std::cout << "run_customized_action" << std::endl;
}

struct SI_case701_timeout_action : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y   = [&ctxt, req]()                     { return InterfaceYStub(ctxt).______sync_y(req); };
        auto do_rpc_rollback = [&ctxt, req](CellBase<bool>& ___1) { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);                                      //seq_id: 1, 2
                    ___action(run_customized_action, ___1->timeout());
        
                    auto ___2 = ___rpc(___ms(10), ___retry(1), do_rpc_rollback, ___1->timeout());       //seq_id: 3, ...
                    auto ___3 = ___rpc(___ms(10), ___retry(1), do_rpc_rollback, ___1->timeout());

        return ___cell(join_rollback_cells, ___1, ___2, ___3);
    }
};

TEST_F(MsgRpcTest, should_able_to_support__SI_with_rollback_rpc__after__rpc_failed_______case701) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value());
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case701_timeout_action>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                                 , drop_msg_with_seq_id({1, 2, 3}) );
    test_thread thread_timer(timer_service_id, []{}                                         , not_drop_msg);
}

TEST_F(MsgRpcTest, should_able_to_support__SI_with_rollback_rpc___do_not_rollback__if_rpc_succeeded_______case7011) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(true, ___r.has_value());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case701_timeout_action>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                                 , not_drop_msg);
    test_thread thread_timer(timer_service_id, []{}                                         , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case702_cancel_timer_after_success : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);  //seq_id: 1, 2
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__cancel_timer_after__retry_rpc_succeeded_______case702) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(true, ___r.has_value());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case702_cancel_timer_after_success>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                                             , not_drop_msg);
    test_thread thread_timer(timer_service_id, []{}                                                     , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case8 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = ___rpc(___ms(10), ___retry(2), do_rpc_sync_y);  //seq_id: 1, 2, 3
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_rpc_with_timeout_and_retry___and_got_result_from_retry_______case8) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(true, ___r.has_value());
        EXPECT_EQ(RpcResult::succeeded, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case8>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                , drop_msg_with_seq_id({1, 2}) );
    test_thread thread_timer(timer_service_id, []{}                        , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case900 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() {
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto do_rpc_sync_y_after_1 = [&ctxt, req](Cell<ResponseBar>& rsp) {
            if (rsp.has_error()) {
                return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, rsp.failed_reason());
            }
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto do_rpc_sync_y_after_2 = [&ctxt, req](Cell<ResponseBar>& rsp) {
            if (rsp.has_error()) {
                return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, rsp.failed_reason());
            }
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);
        auto ___2 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y_after_1, ___1);
        auto ___3 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y_after_2, ___2);
        auto ___4 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y_after_2, ___3);
        return ___4;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__timeout_propagation__through_sequential_rpcs_______case900) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value());
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case900>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                  , drop_all_msg);
    test_thread thread_timer(timer_service_id, []{}                          , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case1000 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() {
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto do_rpc_sync_y_after_1_2 = [&ctxt, req](Cell<ResponseBar>& ___1, Cell<ResponseBar>& ___2) -> Cell<demo::ResponseBar>* {
            if (___1.is_empty() || ___2.is_empty()) {
                return nullptr;
            }

            if (___1.has_error() || ___2.has_error()) {
                return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, ___1.failed_reason());
            }

            return InterfaceYStub(ctxt).______sync_y(req);
        };
        
        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);                                       //seq_id: 1, 3
        auto ___2 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);                                       //seq_id: 2, 4
        auto ___3 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y_after_1_2, ___1, ___2);
        return ___3;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__timeout_propagation______case1000) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case1000>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                   , drop_msg_with_seq_id({1, 2, 3, 4}));
    test_thread thread_timer(timer_service_id, []{}                           , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case1100 : SIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() {
            return InterfaceYStub(ctxt).______sync_y_failed_immediately(req);
        };

        auto do_rpc_sync_y__after__1 = [&ctxt, req](Cell<ResponseBar>& ___1) -> Cell<demo::ResponseBar>* {
            if (___1.has_error()) {
                return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, ___1.failed_reason());
            }

            return InterfaceYStub(ctxt).______sync_y_failed_immediately(req);
        };

        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);
        auto ___2 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y__after__1, ___1);
        return ___2;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__timeout_propagation______case1100) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value());
        EXPECT_EQ(RpcResult::failed, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case1100>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                   , not_drop_msg);
    test_thread thread_timer(timer_service_id, []{}                           , not_drop_msg);
}


#pragma clang diagnostic pop