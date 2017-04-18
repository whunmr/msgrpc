#include <iostream>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <msgrpc/thrift_struct/thrift_codec.h>
#include <type_traits>
#include <future>
#include <atomic>
#include <msgrpc/util/type_traits.h>

#include "demo/demo_api_declare.h"

//TODO: refactor long long as unsigned long long, and typedef to timeout_len_t for timer funcs
////////////////////////////////////////////////////////////////////////////////

#include <msgrpc/core/typedefs.h>
#include <msgrpc/core/adapter/timer_adapter.h>
#include <msgrpc/core/rpc_sequence_id.h>
#include <msgrpc/core/adapter/config.h>
#include <test/core/adapter/simple_timer_adapter.h>
#include <test/test_util/UdpChannel.h>
#include <msgrpc/core/cell/rsp_sink.h>
#include <msgrpc/core/rsp/rsp_dispatcher.h>
#include <msgrpc/core/cell/cell.h>
#include <msgrpc/core/cell/derived_cell.h>
#include <msgrpc/core/cell/timeout_cell.h>

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {
    struct IfaceImplBase {
        virtual msgrpc::RpcResult onRpcInvoke( const msgrpc::ReqMsgHeader& msg_header
                , const char* msg, size_t len
                , msgrpc::RspMsgHeader& rsp_header
                , msgrpc::service_id_t& sender_id) = 0;
    };

    struct IfaceRepository : msgrpc::Singleton<IfaceRepository> {
        void add_iface_impl(iface_index_t ii, IfaceImplBase* iface) {
            assert(iface != nullptr && "interface implementation can not be null");
            assert(___m.find(ii) == ___m.end() && "interface can only register once");
            ___m[ii] = iface;
        }

        IfaceImplBase* get_iface_impl_by(iface_index_t ii) {
            auto iter = ___m.find(ii);
            return iter == ___m.end() ? nullptr : iter->second;
        }

    private:
        std::map<iface_index_t, IfaceImplBase*> ___m;
    };

    struct MsgSender {
        static void send_msg_with_header(const msgrpc::service_id_t& service_id, const RspMsgHeader &rsp_header, const uint8_t *pout_buf, uint32_t out_buf_len) {
            msg_id_t rsp_msg_type = Config::instance().response_msg_id_;

            if (pout_buf == nullptr || out_buf_len == 0) {
                Config::instance().msg_channel_->send_msg(service_id, rsp_msg_type, (const char*)&rsp_header, sizeof(rsp_header));
                return;
            }

            size_t rsp_len_with_header = sizeof(rsp_header) + out_buf_len;
            char *mem = (char *) malloc(rsp_len_with_header);
            if (mem != nullptr) {
                memcpy(mem, &rsp_header, sizeof(rsp_header));
                memcpy(mem + sizeof(rsp_header), pout_buf, out_buf_len);
                Config::instance().msg_channel_->send_msg(service_id, rsp_msg_type, mem, rsp_len_with_header);
                free(mem);
            }
        }
    };

    struct RpcReqMsgHandler {
        static void on_rpc_req_msg(msgrpc::msg_id_t msg_id, const char *msg, size_t len) {
            msg_id_t req_msg_type = Config::instance().request_msg_id_;

            assert(msg_id == req_msg_type && "invalid msg id for rpc");

            if (len < sizeof(msgrpc::ReqMsgHeader)) {
                std::cout << "invalid msg: without sufficient msg header info." << std::endl;
                return;
            }

            auto *req_header = (msgrpc::ReqMsgHeader *) msg;
            msg += sizeof(msgrpc::ReqMsgHeader);

            msgrpc::RspMsgHeader rsp_header;
            rsp_header.msgrpc_version_ = req_header->msgrpc_version_;
            rsp_header.iface_index_in_service_ = req_header->iface_index_in_service_;
            rsp_header.method_index_in_interface_ = req_header->method_index_in_interface_;
            rsp_header.sequence_id_ = req_header->sequence_id_;

            //TODO: add sender info query method
            msgrpc::service_id_t sender_id = req_header->iface_index_in_service_ == 2 ? x_service_id : y_service_id;

            IfaceImplBase *iface = IfaceRepository::instance().get_iface_impl_by(req_header->iface_index_in_service_);
            if (iface == nullptr) {
                rsp_header.rpc_result_ = RpcResult::iface_not_found;

                msg_id_t rsp_msg_type = Config::instance().response_msg_id_;
                msgrpc::Config::instance().msg_channel_->send_msg(sender_id, rsp_msg_type, (const char *) &rsp_header, sizeof(rsp_header));
                return;
            }

            RpcResult ret = iface->onRpcInvoke(*req_header, msg, len - sizeof(msgrpc::ReqMsgHeader), rsp_header, sender_id);

            if (ret == RpcResult::failed || ret == RpcResult::method_not_found) {
                return MsgSender::send_msg_with_header(sender_id, rsp_header, nullptr, 0);
            }

            //TODO: using pipelined processor to handling input/output msgheader and rpc statistics.
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {

    template<typename RSP>
    static RpcResult send_rsp_cell_value(const service_id_t& sender_id, const RspMsgHeader &rsp_header, const Cell<RSP>& rsp_cell) {
        if (!rsp_cell.has_value_) {
            return RpcResult::failed;
        }

        uint8_t* pout_buf = nullptr;
        uint32_t out_buf_len = 0;
        if (!ThriftEncoder::encode(rsp_cell.value(), &pout_buf, &out_buf_len)) {
            std::cout << "encode failed on remtoe side." << std::endl;
            return RpcResult::failed;
        }

        MsgSender::send_msg_with_header(sender_id, rsp_header, pout_buf, out_buf_len);
        return RpcResult::succeeded;
    }

    template<typename T, iface_index_t iface_index>
    struct InterfaceImplBaseT : IfaceImplBase {
        InterfaceImplBaseT() {
            IfaceRepository::instance().add_iface_impl(iface_index, this);
        }

        template<typename REQ, typename RSP>
        RpcResult invoke_templated_method(msgrpc::Cell<RSP>* (T::*method_impl)(const REQ&)
                , const char *msg, size_t len
                , msgrpc::service_id_t& sender_id
                , msgrpc::RspMsgHeader& rsp_header) {

            REQ req;
            if (! ThriftDecoder::decode(req, (uint8_t *) msg, len)) {
                std::cout << "decode failed on remote side." << std::endl;
                return RpcResult::failed;
            }

            msgrpc::Cell<RSP>* rsp_cell = ((T*)this->*method_impl)(req);
            if ( rsp_cell == nullptr ) {
                //TODO: log call failed
                return RpcResult::failed;
            }

            if (rsp_cell->has_value_) {
                RpcResult ret = send_rsp_cell_value(sender_id, rsp_header, *rsp_cell);
                delete rsp_cell;
                return ret;
            }

            auto final_action = derive_final_action([sender_id, rsp_header](msgrpc::Cell<RSP>& r) {
                if (r.has_value_) {
                    send_rsp_cell_value(sender_id, rsp_header, r);
                } else {
                    //TODO: handle error case where result do not contains value. maybe timeout?
                }
            }, rsp_cell);

            return RpcResult::succeeded;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {

    struct RpcStubBase {
        RpcStubBase(RpcContext& ctxt) : ctxt_(ctxt) { }
        RpcContext& ctxt_;

        //TODO: split into .h and .cpp
        bool send_rpc_request_buf( msgrpc::iface_index_t iface_index, msgrpc::method_index_t method_index
                                 , const uint8_t *pbuf, uint32_t len, RspSink* rpc_rsp_cell_sink) const {
            size_t msg_len_with_header = sizeof(msgrpc::ReqMsgHeader) + len;

            char *mem = (char *) malloc(msg_len_with_header);
            if (!mem) {
                std::cout << "alloc mem failed, during sending rpc request." << std::endl;
                return false;
            }

            auto seq_id = msgrpc::RpcSequenceId::instance().get();
            msgrpc::RspDispatcher::instance().register_rsp_Handler(seq_id, rpc_rsp_cell_sink);

            auto header = (msgrpc::ReqMsgHeader *) mem;
            header->msgrpc_version_ = 0;
            header->iface_index_in_service_ = iface_index;
            header->method_index_in_interface_ = method_index;
            header->sequence_id_ = seq_id;
            memcpy(header + 1, (const char *) pbuf, len);

            //std::cout << "stub sending msg with length: " << msg_len_with_header << std::endl;
            //TODO: find y_service_id by interface name "IBuzzMath"
            msgrpc::service_id_t service_id = iface_index == 1 ? x_service_id : y_service_id;

            msg_id_t req_msg_type = msgrpc::Config::instance().request_msg_id_;
            bool send_ret = msgrpc::Config::instance().msg_channel_->send_msg(service_id, req_msg_type, mem, msg_len_with_header);
            free(mem);

            return send_ret;
        }

        template<typename T, typename U>
        Cell<U>* encode_request_and_send(msgrpc::iface_index_t iface_index, msgrpc::method_index_t method_index, const T &req) const {
            uint8_t* pbuf;
            uint32_t len;
            /*TODO: extract interface for encode/decode for other protocol adoption such as protobuf*/
            if (!ThriftEncoder::encode(req, &pbuf, &len)) {
                /*TODO: how to do with log, maybe should extract logging interface*/
                std::cout << "encode failed." << std::endl;
                return nullptr; //TODO: return false;
            }

            Cell<U>* rpc_result_cell = new Cell<U>();

            if (! send_rpc_request_buf(iface_index, method_index, pbuf, len, rpc_result_cell)) {
                delete rpc_result_cell;
                return nullptr;
            }

            return ctxt_.track(rpc_result_cell);
        };
    };
}

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {
    /*SI is short for service interaction*/
    template<typename T, typename U>
    struct MsgRpcSIBase {
        msgrpc::Cell<U> *run_nested_si(const T &req, RpcContext& ctxt) {
            return do_run(req, ctxt);
        }

        msgrpc::Cell<U> *run(const T &req) {
            msgrpc::RpcContext *ctxt = new msgrpc::RpcContext();

            msgrpc::Cell<U> *result_cell = do_run(req, *ctxt);
            result_cell->set_binded_context(*ctxt);
            ctxt->release_list_.remove(result_cell);

            return result_cell;
        }

        virtual msgrpc::Cell<U> *do_run(const T &req, msgrpc::RpcContext& ctxt) = 0;
    };
}

////////////////////////////////////////////////////////////////////////////////
//interface implementation related elements:
namespace msgrpc {
    template<typename REQ, typename RSP>
    msgrpc::Cell<RSP>* call_sync_impl(void(*f)(const REQ &, RSP &), const REQ &req) {
        auto* rsp_cell = new msgrpc::Cell<RSP>();

        RSP rsp;
        f(req, rsp);

        rsp_cell->set_value(rsp);
        return rsp_cell;
    }

    template<typename T>
    Cell<T>* failed_cell_with_reason(RpcContext &ctxt, const RpcResult& failed_reason) {
        Cell<T>* cell = new Cell<T>();
        cell->set_failed_reason(failed_reason);
        ctxt.track(cell);
        return cell;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <test/core/adapter/udp_msg_channel.h>
#include <test/details/set_timer_handler.h>

using namespace demo;

//constants for testing.
const int k_req_init_value = 1;
const int k__sync_y__delta = 3;
const int k__sync_x__delta = 17;

namespace demo {
    struct RpcTimeoutHandler : msgrpc::ThreadLocalSingleton<RpcTimeoutHandler> {
        void on_timeout(const char* msg, size_t len) {
            assert(msg != nullptr && len == sizeof(timer_info));
            timer_info& ti = *(timer_info*)msg;

            msgrpc::rpc_sequence_id_t seq_id =  (msgrpc::rpc_sequence_id_t)((uintptr_t)(ti.user_data_));

            msgrpc::RspDispatcher::instance().on_rsp_handler_timeout(seq_id);
        }
    };
}

void msgrpc_loop(unsigned short udp_port, std::function<void(void)> init_func, std::function<bool(const char* msg, size_t len)> should_drop) {
    msgrpc::Config::instance().init_with( &UdpMsgChannel::instance()
                                        , &SimpleTimerAdapter::instance()
                                        , k_msgrpc_request_msg_id
                                        , k_msgrpc_response_msg_id
                                        , k_msgrpc_set_timer_msg
                                        , k_msgrpc_timeout_msg);

    test_service::instance().current_service_id_ = udp_port;

    UdpChannel channel(udp_port,
        [&init_func, udp_port, &should_drop](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                return init_func();
            } else if (msg_id == msgrpc::Config::instance().request_msg_id_) {
                if (! should_drop(msg, len)) {
                    return msgrpc::RpcReqMsgHandler::on_rpc_req_msg(msg_id, msg, len);
                }
            } else if (msg_id == msgrpc::Config::instance().response_msg_id_) {
                return msgrpc::RspDispatcher::instance().handle_rpc_rsp(msg_id, msg, len);
            } else if (msg_id == msgrpc::Config::instance().set_timer_msg_id_) {
                return demo::SetTimerHandler::instance().set_timer(msg, len);
            } else if (msg_id == msgrpc::Config::instance().timeout_msg_id_) {
                if (! TimerMgr::instance().should_ignore(msg, len)) {
                    return demo::RpcTimeoutHandler::instance().on_timeout(msg, len);
                }
            } else {
                std::cout << "got unknow msg with id: " << msg_id << std::endl;
            }
        }
    );
}

////////////////////////////////////////////////////////////////////////////////
//TODO: define following macros:
#define declare_interface_on_consumer
#define  define_interface_on_consumer
#define declare_interface_on_provider
#define  define_interface_on_provider

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------generate by:  declare and define stub macros
struct InterfaceXStub : msgrpc::RpcStubBase {
    using msgrpc::RpcStubBase::RpcStubBase;
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
InterfaceXImpl interfaceXImpl;
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
//---------------- implement interface in here:
void ______sync_x_impl(const RequestFoo& req, ResponseBar& rsp) {
    rsp.__set_rspa(req.reqa + k__sync_x__delta);
}

msgrpc::Cell<ResponseBar>* InterfaceXImpl::______sync_x(const RequestFoo& req) {
    std::cout << "                     ______sync_x" << std::endl;
    return msgrpc::call_sync_impl(______sync_x_impl, req);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------generate by:  declare and define stub macros
struct InterfaceYStub : msgrpc::RpcStubBase {
    using msgrpc::RpcStubBase::RpcStubBase;

    msgrpc::Cell<ResponseBar>* ______sync_y(const RequestFoo&);
    msgrpc::Cell<ResponseBar>* _____async_y(const RequestFoo&);
    msgrpc::Cell<ResponseBar>* ______sync_y_failed(const RequestFoo&);
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

////////////////////////////////////////////////////////////////////////////////
//---------------- generate this part by macros set:
struct InterfaceYImpl : msgrpc::InterfaceImplBaseT<InterfaceYImpl, 2> {
    msgrpc::Cell<ResponseBar>* ______sync_y(const RequestFoo& req);
    msgrpc::Cell<ResponseBar>* _____async_y(const RequestFoo& req);
    msgrpc::Cell<ResponseBar>* ______sync_y_failed(const RequestFoo& req);

    virtual msgrpc::RpcResult onRpcInvoke( const msgrpc::ReqMsgHeader& msg_header
            , const char* msg, size_t len
            , msgrpc::RspMsgHeader& rsp_header
            , msgrpc::service_id_t& sender_id) override;
};

////////////////////////////////////////////////////////////////////////////////
//---------------- generate this part by macros set: interface_implement_define.h
InterfaceYImpl interfaceYImpl;
msgrpc::RpcResult InterfaceYImpl::onRpcInvoke( const msgrpc::ReqMsgHeader& req_header, const char* msg
                                             , size_t len, msgrpc::RspMsgHeader& rsp_header
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
void ______sync_y_impl(const RequestFoo& req, ResponseBar& rsp) {
    rsp.__set_rspa(req.reqa + k__sync_y__delta);
}

msgrpc::Cell<ResponseBar>* InterfaceYImpl::______sync_y(const RequestFoo& req) {
    std::cout << "                     ______sync_y" << std::endl;
    return msgrpc::call_sync_impl(______sync_y_impl, req);
}


struct SI_____async_y : msgrpc::MsgRpcSIBase<RequestFoo, ResponseBar> {
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
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace msgrpc;

bool can_safely_exit = false;
std::mutex can_safely_exit_mutex;
std::condition_variable can_safely_exit_cv;

#include <condition_variable>
#include <include/msgrpc/core/cell/derived_action.h>

struct MsgRpcTest : public ::testing::Test {
    virtual void SetUp() {
        can_safely_exit = false;

        RpcSequenceId::instance().reset();
        TimerMgr::instance().reset();
    }

    virtual void TearDown() {
        std::unique_lock<std::mutex> lk(can_safely_exit_mutex);
        can_safely_exit_cv.wait(lk, []{return can_safely_exit;});
    }
};

struct test_thread : std::thread {
    template<typename... Args>
    test_thread(Args... args) : std::thread(msgrpc_loop, args...) { /**/ }

    ~test_thread() {
        join();
    }
};

////////////////////////////////////////////////////////////////////////////////
void create_delayed_exiting_thread() {
    std::thread thread_delayed_exiting(
            []{
                std::this_thread::sleep_for(std::chrono::milliseconds(200));

                std::lock_guard<std::mutex> lk(can_safely_exit_mutex);
                can_safely_exit = true;
                can_safely_exit_cv.notify_one();

                UdpChannel::close_all_channels();
            });
    thread_delayed_exiting.detach();
}

template<typename SI>
void rpc_main(std::function<void(Cell<ResponseBar>&)> f) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    RequestFoo foo; foo.reqa = k_req_init_value;

    auto* rsp_cell = SI().run(foo);

    if (rsp_cell != nullptr) {
        derive_final_action([f](Cell<ResponseBar>& r) {
            f(r);
            create_delayed_exiting_thread();
        }, rsp_cell);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
auto not_drop_msg = [](const char* msg, size_t len) {
    return false;
};

auto drop_all_msg = [](const char* msg, size_t len) {
    return true;
};

auto drop_msg_with_seq_id(std::initializer_list<int> seq_ids_to_drop) -> std::function<bool(const char*, size_t)> {
    return [seq_ids_to_drop](const char* msg, size_t len) -> bool {
        assert(len >= sizeof(msgrpc::ReqMsgHeader));
        msgrpc::ReqMsgHeader* req = (msgrpc::ReqMsgHeader*)(msg);
        for (int seq_to_drop : seq_ids_to_drop) {
            if (req->sequence_id_ == seq_to_drop) {
                return true;
            }
        }

        return false;
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case100 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        return InterfaceYStub(ctxt)._____async_y(req);
    }
};

TEST_F(MsgRpcTest, rpc__should_able_to_support___SI_with_single_rpc___case100) {
    // x ----(req1)-------------------------->y  (async_y)
    //        x (sync_x) <=========(req2)=====y  (async_y)
    //        x (sync_x) ==========(rsp2)====>y  (async_y)
    // x <---(rsp1)---------------------------y  (async_y)
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value_);
        EXPECT_EQ(k_req_init_value + k__sync_x__delta, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case100>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void save_rsp_from_other_services_to_db(Cell<ResponseBar>& r) { std::cout << "1/2 ----------------->>>> write db." << std::endl; };
void save_rsp_to_log(Cell<ResponseBar>& r)                    { std::cout << "2/2 ----------------->>>> save_log." << std::endl; };

struct SI_case200 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
                    ___action(save_rsp_from_other_services_to_db, ___1);
                    ___action(save_rsp_to_log, ___1);
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to_support___SI_with_single_rpc____which_bind_with_actions______________case200) {
    // x ----(req)---->y (sync_y)
    // x <---(rsp)-----y
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value_);
        EXPECT_EQ(k_req_init_value + k__sync_y__delta, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&] {rpc_main<SI_case200>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void merge_logic(Cell<ResponseBar>& result, Cell<ResponseBar>& cell_1, Cell<ResponseBar>& cell_2)  {
    if (cell_1.has_value_ && cell_2.has_value_) {
        ResponseBar bar;
        bar.rspa = cell_1.value().rspa + cell_2.value().rspa;
        result.set_value(bar);
    }
};

struct SI_case300 : MsgRpcSIBase<RequestFoo, ResponseBar> {
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
        EXPECT_TRUE(___r.has_value_);
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
struct SI_case4001 : MsgRpcSIBase<RequestFoo, ResponseBar> {
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
        EXPECT_TRUE(___r.has_value_);
        EXPECT_EQ(k_req_init_value + k__sync_y__delta * 3, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4001>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                   , not_drop_msg );
    test_thread thread_timer(timer_service_id, []{}                           , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case4011_failed : MsgRpcSIBase<RequestFoo, ResponseBar> {
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
        EXPECT_EQ(false, ___r.has_value_);
        EXPECT_EQ(RpcResult::failed, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4011_failed>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                          , not_drop_msg );
    test_thread thread_timer(timer_service_id, []{}                                  , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case4021_failed : MsgRpcSIBase<RequestFoo, ResponseBar> {
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
        EXPECT_EQ(false, ___r.has_value_);
        EXPECT_EQ(RpcResult::failed, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4021_failed>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                          , not_drop_msg );
    test_thread thread_timer(timer_service_id, []{}                                  , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gen2(Cell<ResponseBar> &result, Cell<ResponseBar> &rsp_cell_1)  {
    if (rsp_cell_1.has_value_) {
        result.set_value(rsp_cell_1);
    }
};

void action1(Cell<ResponseBar> &r) { std::cout << "1/1 ----------------->>>> action1." << std::endl; };

struct SI_case500 : MsgRpcSIBase<RequestFoo, ResponseBar> {
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
        EXPECT_TRUE(___r.has_value_);
        int expect_value = (k_req_init_value + k__sync_x__delta) * 2;
        EXPECT_EQ(expect_value, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case500>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gen6(Cell<ResponseBar> &result, Cell<ResponseBar> &rsp)  {
    if (rsp.has_error()) {
        return result.set_failed_reason(rsp.failed_reason());
    }

    return result.set_value(rsp);
};

struct SI_case600 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y_failed(req);
        return ___cell(gen6, ___1);
    }
};

TEST_F(MsgRpcTest, should_able_to_support_failure_propagation__during__bind_cell_____case600) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case600>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TODO: add service discovery

//TODO: handle concor case: when a detached rpc's cell are not in dependency graph of final result cell,
//      if the result cell finished, can not release response handler of the detached cell.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case700_timeout : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__rpc_with_timer_and_retry___case700) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value_);
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case700_timeout>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                          , drop_all_msg);
    test_thread thread_timer(timer_service_id, []{}                                  , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void join_rollback_cells(Cell<ResponseBar> &result, Cell<ResponseBar> &___1, Cell<ResponseBar> &___2, Cell<ResponseBar> &___3)  {
    if (___1.has_value()) {
        return result.set_cell_value(___1);
    }

    if (___2.has_value_or_error() && ___3.has_value_or_error()) {
        result.set_cell_value(___1);
    }
};

void run_customized_action(CellBase<bool> &r) {
    std::cout << "run_customized_action" << std::endl;
}

struct SI_case701_timeout_action : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y   = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };
        auto do_rpc_rollback = [&ctxt, req](CellBase<bool>& ___1) { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);  //seq_id: 1, 2
                    ___action(run_customized_action, ___1->timeout());

                    auto ___2 = ___rpc(___ms(10), ___retry(1), do_rpc_rollback, ___1->timeout()); //seq_id: 3, ...
                    auto ___3 = ___rpc(___ms(10), ___retry(1), do_rpc_rollback, ___1->timeout());

        return ___cell(join_rollback_cells, ___1, ___2, ___3);
    }
};

TEST_F(MsgRpcTest, should_able_to_support__SI_with_rollback_rpc__after__rpc_failed_______case701) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value_);
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case701_timeout_action>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                                 , drop_msg_with_seq_id({1, 2, 3}) );
    test_thread thread_timer(timer_service_id, []{}                                         , not_drop_msg);
}

TEST_F(MsgRpcTest, should_able_to_support__SI_with_rollback_rpc___do_not_rollback__if_rpc_succeeded_______case7011) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(true, ___r.has_value_);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case701_timeout_action>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                                 , not_drop_msg);
    test_thread thread_timer(timer_service_id, []{}                                         , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case702_cancel_timer_after_success : MsgRpcSIBase<RequestFoo, ResponseBar> {
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
struct SI_case8 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = ___rpc(___ms(10), ___retry(2), do_rpc_sync_y);  //seq_id: 1, 2, 3
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_rpc_with_timeout_and_retry___and_got_result_from_retry_______case8) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(true, ___r.has_value_);
        EXPECT_EQ(RpcResult::succeeded, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case8>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                , drop_msg_with_seq_id({1, 2}) );
    test_thread thread_timer(timer_service_id, []{}                        , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case900 : MsgRpcSIBase<RequestFoo, ResponseBar> {
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
        EXPECT_EQ(false, ___r.has_value_);
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case900>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                  , drop_all_msg);
    test_thread thread_timer(timer_service_id, []{}                          , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case1000 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() {
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto do_rpc_sync_y_after_1 = [&ctxt, req](Cell<ResponseBar>& ___1, Cell<ResponseBar>& ___2) -> Cell<demo::ResponseBar>* {
            if (___1.is_empty() || ___2.is_empty()) {
                return nullptr;
            }

            if (___1.has_error() || ___2.has_error()) {
                return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, ___1.failed_reason());
            }

            return InterfaceYStub(ctxt).______sync_y(req);
        };
        
        auto ___1 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);     //seq_id: 1, 3
        auto ___2 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y);     //seq_id: 2, 4
        auto ___3 = ___rpc(___ms(10), ___retry(1), do_rpc_sync_y_after_1, ___1, ___2);
        return ___3;
    }
};

TEST_F(MsgRpcTest, should_able_to_support__timeout_propagation______case1000) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_EQ(false, ___r.has_value_);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case1000>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                   , drop_msg_with_seq_id({1, 2, 3, 4}));
    test_thread thread_timer(timer_service_id, []{}                           , not_drop_msg);
}
