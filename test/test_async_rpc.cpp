#include <iostream>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <msgrpc/thrift_struct/thrift_codec.h>
#include <msgrpc/frp/cell.h>
#include <type_traits>
#include <future>

using namespace std;
using namespace std::chrono;

#include "demo/demo_api_declare.h"
////////////////////////////////////////////////////////////////////////////////
//TODO: check valgrind check results
namespace msgrpc {
    template <typename T> struct Ret {};

    typedef unsigned short msg_id_t;
    typedef unsigned short service_id_t; //TODO: how to deal with different service id types

    struct MsgChannel {
        //TODO: extract common channel interface
        virtual bool send_msg(const service_id_t& remote_service_id, msg_id_t msg_id, const char* buf, size_t len) const = 0;
    };

    struct Config {
        void init_with(MsgChannel *msg_channel, msgrpc::msg_id_t request_msg_id, msgrpc::msg_id_t response_msg_id) {
            instance().msg_channel_ = msg_channel;
            request_msg_id_  = request_msg_id;
            response_msg_id_ = response_msg_id;
        }

        static inline Config& instance() {
            static thread_local Config instance;
            return instance;
        }

        MsgChannel* msg_channel_;
        msg_id_t request_msg_id_;
        msg_id_t response_msg_id_;
    };
}

namespace msgrpc {
    typedef uint32_t rpc_sequence_id_t;
    struct RpcSequenceId : msgrpc::ThreadLocalSingleton<RpcSequenceId> {
        rpc_sequence_id_t get() {
            return sequence_id_++;
        }

    private:
        //TODO: using atomic int
        rpc_sequence_id_t sequence_id_ = {0};
    };
}

namespace msgrpc {

    typedef uint8_t  method_index_t;
    typedef uint16_t iface_index_t;

    struct MsgHeader {
        uint8_t           msgrpc_version_ = {0};
        method_index_t    method_index_in_interface_ = {0};
        iface_index_t     iface_index_in_service_ = {0};
        rpc_sequence_id_t sequence_id_;
        //TODO: unsigned char  feature_id_in_service_ = {0};
        //TODO: TLV encoded varient length options
        //TODO: if not encoded/decode, how to deal hton and ntoh
        //unsigned long  sequence_no_;
        //optional 1
        /*TODO: call sequence number*/
    };

    /*TODO: consider make msgHeader encoded through thrift*/
    struct ReqMsgHeader : MsgHeader {
    };

    enum class RpcResult : unsigned short {
        succeeded = 0
        , failed  = 1
        , method_not_found = 2
        , iface_not_found =  3
    };

    struct RspMsgHeader : MsgHeader {
        RpcResult rpc_result_ = { RpcResult::succeeded };
    };
}

////////////////////////////////////////////////////////////////////////////////
#include "test_util/UdpChannel.h"
namespace demo {
    const msgrpc::msg_id_t k_msgrpc_request_msg_id = 101;
    const msgrpc::msg_id_t k_msgrpc_response_msg_id = 102;

    struct UdpMsgChannel : msgrpc::MsgChannel, msgrpc::Singleton<UdpMsgChannel> {
        virtual bool send_msg(const msgrpc::service_id_t& remote_service_id, msgrpc::msg_id_t msg_id, const char* buf, size_t len) const {
            size_t msg_len_with_msgid = sizeof(msgrpc::msg_id_t) + len;
            char* mem = (char*)malloc(msg_len_with_msgid);
            if (mem) {
                *(msgrpc::msg_id_t*)(mem) = msg_id;
                memcpy(mem + sizeof(msgrpc::msg_id_t), buf, len);
                //cout << "send msg len: " << msg_len_with_msgid << endl;
                g_msg_channel->send_msg_to_remote(string(mem, msg_len_with_msgid), udp::endpoint(udp::v4(), remote_service_id));
                free(mem);
            } else {
                cout << "send msg failed: allocation failure." << endl;
            }
            return true;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
using namespace demo;

const msgrpc::service_id_t k_remote_service_id = 2222;
const msgrpc::service_id_t k_local_service_id  = 3333;


////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {

    struct IfaceImplBase {
        virtual bool onRpcInvoke( const msgrpc::ReqMsgHeader& msg_header
                                , const char* msg, size_t len
                                , msgrpc::RspMsgHeader& rsp_header
                                , uint8_t*& pout_buf, uint32_t& out_buf_len) = 0;
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

    template<typename T, iface_index_t iface_index>
    struct InterfaceImplBaseT : IfaceImplBase {
        InterfaceImplBaseT() {
            IfaceRepository::instance().add_iface_impl(iface_index, this);
        }

        template<typename REQ, typename RSP>
        bool invoke_templated_method( bool (T::*method_impl)(const REQ&, RSP&)
                                    , const char *msg, size_t len
                                    , uint8_t *&pout_buf, uint32_t &out_buf_len) {
            REQ req;
            if (! ThriftDecoder::decode(req, (uint8_t *) msg, len)) {
                cout << "decode failed on remote side." << endl;
                return false;
            }

            RSP rsp;
            if (! ((T*)this->*method_impl)(req, rsp)) {
                //TODO: log
                return false;
            }

            if (! ThriftEncoder::encode(rsp, &pout_buf, &out_buf_len)) {
                cout << "encode failed on remtoe side." << endl;
                return false;
            }

            return true;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {
    struct RpcReqMsgHandler {
        static void on_rpc_req_msg(msgrpc::msg_id_t msg_id, const char *msg, size_t len) {
            //cout << "remote received msg with length: " << len << endl;
            assert(msg_id == k_msgrpc_request_msg_id && "invalid msg id for rpc");

            if (len < sizeof(msgrpc::ReqMsgHeader)) {
                cout << "invalid msg: without sufficient msg header info." << endl;
                return;
            }

            auto *req_header = (msgrpc::ReqMsgHeader *) msg;
            msg += sizeof(msgrpc::ReqMsgHeader);

            msgrpc::RspMsgHeader rsp_header;
            rsp_header.msgrpc_version_ = req_header->msgrpc_version_;
            rsp_header.iface_index_in_service_ = req_header->iface_index_in_service_;
            rsp_header.method_index_in_interface_ = req_header->method_index_in_interface_;
            rsp_header.sequence_id_ = req_header->sequence_id_;

            uint8_t *pout_buf;
            uint32_t out_buf_len;

            IfaceImplBase *iface = IfaceRepository::instance().get_iface_impl_by(req_header->iface_index_in_service_);
            if (iface == nullptr) {
                rsp_header.rpc_result_ = RpcResult::iface_not_found;
                msgrpc::Config::instance().msg_channel_->send_msg(k_local_service_id, k_msgrpc_response_msg_id, (const char *) &rsp_header, sizeof(rsp_header));
                return;
            }

            bool ret = iface->onRpcInvoke(*req_header, msg, len - sizeof(msgrpc::ReqMsgHeader), rsp_header, pout_buf, out_buf_len);
            if (ret) {
                return send_msg_with_header(rsp_header, pout_buf, out_buf_len);
            } else {
                msgrpc::Config::instance().msg_channel_->send_msg(k_local_service_id, k_msgrpc_response_msg_id, (const char *) &rsp_header, sizeof(rsp_header));
            }

            //TODO: using pipelined processor to handling input/output msgheader and rpc statistics.
        }

        static void send_msg_with_header(RspMsgHeader &rsp_header, const uint8_t *pout_buf, uint32_t out_buf_len) {
            size_t rsp_len_with_header = sizeof(rsp_header) + out_buf_len;
            char *mem = (char *) malloc(rsp_len_with_header);
            if (mem != nullptr) {
                memcpy(mem, &rsp_header, sizeof(rsp_header));
                memcpy(mem + sizeof(rsp_header), pout_buf, out_buf_len);
                /*TODO: replace k_local_service_id to sender id*/
                Config::instance().msg_channel_->send_msg(k_local_service_id, k_msgrpc_response_msg_id, mem, rsp_len_with_header);
                free(mem);
            }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {

    struct RpcRspCellSink {
        virtual bool set_rpc_rsp(RspMsgHeader* rsp_header, const char* msg, size_t len) = 0;
    };

    template<typename T>
    struct RpcCell : Cell<T>, RpcRspCellSink {
        void set_binded_context(RpcContext* context) {
            context_ = context;
        }

        RpcContext* context_;
    };

    template<typename T>
    struct RpcRspCell : RpcCell<T> {
        virtual bool set_rpc_rsp(RspMsgHeader* rsp_header, const char* msg, size_t len) override {
            T rsp;
            if (! ThriftDecoder::decode(rsp, (uint8_t *) msg, len)) {
                cout << "decode failed on remote side." << endl;
                return false;
            }

            //TODO: handle msg header status
            Cell<T>::set_value(std::move(rsp));
            return true;
        }
    };

    template<typename VT, typename... T>
    struct DerivedAction : Updatable {
        DerivedAction(bool is_final_action, std::function<VT(T...)> logic, T &&... args)
                : is_final_action_(is_final_action), bind_(logic, std::forward<T>(args)...) {
            call_each_args(std::forward<T>(args)...);
        }

        DerivedAction(std::function<VT(T...)> logic, T &&... args) : bind_(logic, std::forward<T>(args)...) {
            call_each_args(std::forward<T>(args)...);
        }

        template<typename C, typename... Ts>
        void call_each_args(C &&c, Ts &&... args) {
            c->register_listener(this);
            call_each_args(std::forward<Ts>(args)...);
        }

        template<typename C>
        void call_each_args(C &&c) {
            c->register_listener(this);

            if (is_final_action_) {
                if (c->context_) {
                    c->context_->track_item_to_release(this);
                }
                assert(c->context_ != nullptr && "to release context, should bind not null context to final action.");
                context_ = c->context_;
            }
        }

        void update() override {
            bind_();

            if (is_final_action_) {
                delete context_;
            }
        }

        bool is_final_action_ = {false};
        RpcContext* context_;
        using bind_type = decltype(std::bind(std::declval<std::function<VT(T...)>>(), std::declval<T>()...));
        bind_type bind_;
    };


    template<typename F, typename... Args>
    auto derive_action(F &&f, Args &&... args) -> DerivedAction<decltype(f(args...)), Args...>* {
        return new DerivedAction<decltype(f(args...)), Args...>(std::forward<F>(f), std::forward<Args>(args)...);
    }

    template<typename F, typename... Args>
    auto derive_final_action(F &&f, Args &&... args) -> DerivedAction<decltype(f(args...)), Args...>* {
        return new DerivedAction<decltype(f(args...)), Args...>(/*is_final_action=*/true, std::forward<F>(f), std::forward<Args>(args)...);
    }

    template<typename VT, typename... T>
    struct DerivedCell : RpcRspCell<VT> {
        DerivedCell(std::function<boost::optional<VT>(T...)> logic, T &&... args)
                : bind_(logic, std::forward<T>(args)...) {
            call_each_args(std::forward<T>(args)...);
        }

        template<typename C, typename... Ts>
        void call_each_args(C &&c, Ts &&... args) {
            c->register_listener(this);
            call_each_args(std::forward<Ts>(args)...);
        }

        template<typename C>
        void call_each_args(C &&c) {
            c->register_listener(this);
        }

        void update() override {
            if (!Cell<VT>::cell_has_value_) {
                boost::optional<VT> value = bind_();
                if (value) {
                    Cell<VT>::set_value(std::move(value.value()));
                }
            }
        }

        using bind_type = decltype(std::bind(std::declval<std::function<boost::optional<VT>(T...)>>(), std::declval<T>()...));
        bind_type bind_;
    };

    template<typename F, typename... Args>
    auto derive_cell(F &&f, Args &&... args) -> DerivedCell<typename decltype(f(args...))::value_type, Args...>* {
        return new DerivedCell<typename decltype(f(args...))::value_type, Args...>(std::forward<F>(f),
                                                                                   std::forward<Args>(args)...);
    }

    struct RpcRspDispatcher : msgrpc::Singleton<RpcRspDispatcher> {
        void register_rsp_Handler(rpc_sequence_id_t sequence_id, RpcRspCellSink* func) {
            assert(id_func_map_.find(sequence_id) == id_func_map_.end() && "should register with unique id.");
            id_func_map_[sequence_id] = func;
        }

        void handle_rpc_rsp(msgrpc::msg_id_t msg_id, const char *msg, size_t len) {
            //cout << "DEBUG: local received msg----------->: " << string(msg, len) << endl;
            if (len < sizeof(RspMsgHeader)) {
                cout << "WARNING: invalid rsp msg" << endl;
                return;
            }

            auto* rsp_header = (RspMsgHeader*)msg;
            //cout << "                   sequence_id: " << rsp_header->sequence_id_ << endl;

            auto iter = id_func_map_.find(rsp_header->sequence_id_);
            if (iter == id_func_map_.end()) {
                cout << "WARNING: can not find rsp handler" << endl;
                return;
            }

            (iter->second)->set_rpc_rsp(rsp_header, msg + sizeof(RspMsgHeader), len - sizeof(RspMsgHeader));
        }

        std::map<rpc_sequence_id_t, RpcRspCellSink*> id_func_map_;
    };
}

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {

    struct RpcStubBase {
        //TODO: split into .h and .cpp
        bool send_rpc_request_buf( msgrpc::iface_index_t iface_index, msgrpc::method_index_t method_index
                                 , const uint8_t *pbuf, uint32_t len, RpcRspCellSink* rpc_rsp_cell_sink) const {
            size_t msg_len_with_header = sizeof(msgrpc::ReqMsgHeader) + len;

            char *mem = (char *) malloc(msg_len_with_header);
            if (!mem) {
                cout << "alloc mem failed, during sending rpc request." << endl;
                return false;
            }

            auto seq_id = msgrpc::RpcSequenceId::instance().get();
            msgrpc::RpcRspDispatcher::instance().register_rsp_Handler(seq_id, rpc_rsp_cell_sink);

            auto header = (msgrpc::ReqMsgHeader *) mem;
            header->msgrpc_version_ = 0;
            header->iface_index_in_service_ = iface_index;
            header->method_index_in_interface_ = method_index;
            header->sequence_id_ = seq_id;
            memcpy(header + 1, (const char *) pbuf, len);

            //cout << "stub sending msg with length: " << msg_len_with_header << endl;
            //TODO: find k_remote_service_id by interface name "IBuzzMath"
            bool send_ret = msgrpc::Config::instance().msg_channel_->send_msg(k_remote_service_id, k_msgrpc_request_msg_id, mem, msg_len_with_header);
            free(mem);

            return send_ret;
        }

        template<typename T, typename U>
        RpcRspCell<U>* encode_request_and_send(msgrpc::iface_index_t iface_index, msgrpc::method_index_t method_index, const T &req) const {
            uint8_t* pbuf;
            uint32_t len;
            /*TODO: extract interface for encode/decode for other protocol adoption such as protobuf*/
            if (!ThriftEncoder::encode(req, &pbuf, &len)) {
                /*TODO: how to do with log, maybe should extract logging interface*/
                cout << "encode failed." << endl;
                return nullptr; //TODO: return false;
            }

            RpcRspCell<U>* rsp_cell = new RpcRspCell<U>();

            if (! send_rpc_request_buf(iface_index, method_index, pbuf, len, rsp_cell)) {
                delete rsp_cell;
                return nullptr;
            }

            return rsp_cell;
        };
    };
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  1     +1  2
//A --> B --> C
//
//      4  +2
//      B <-- C
//
//7  +3
//A <-- B

#if 0
___def_si(SimpleAsyncSI) {
} simple_service_interaction;
#endif

////////////////////////////////////////////////////////////////////////////////
using namespace demo;

struct ServiceC {
    msgrpc::RpcRspCell<ResponseBar>* calculate_next_prime_value(const RequestFoo& req_value) {
        return new msgrpc::RpcRspCell<ResponseBar>();
    }
};


struct ServiceD {
    msgrpc::RpcRspCell<ResponseBar>* calculate_ddd(const RequestFoo& req_value) {
        return new msgrpc::RpcRspCell<ResponseBar>();
    }
};

msgrpc::RpcRspCell<ResponseBar>* rspc;
msgrpc::RpcRspCell<ResponseBar>* rspd;

struct SimpleAsyncSI {
    static boost::optional<ResponseBar> derive_result_from_c_and_d(msgrpc::Cell<ResponseBar> *c, msgrpc::Cell<ResponseBar> *d) {
        if (c->cell_has_value_ && d->cell_has_value_) {
            ResponseBar bar;
            bar.bara = (c->value_.bara + d->value_.bara) / 2;
            return boost::make_optional(bar);
        }
        return {};
    }

    msgrpc::RpcCell<ResponseBar>* run(const RequestFoo& req) {
        msgrpc::RpcContext* ctxt = new msgrpc::RpcContext();

        rspc = ServiceC().calculate_next_prime_value(req); //TODO: check nullptr when sending rpc request failed
        ctxt->track_item_to_release(rspc);

        rspd = ServiceD().calculate_ddd(req);
        ctxt->track_item_to_release(rspd);

        auto si_result = msgrpc::derive_cell(derive_result_from_c_and_d,  rspc, rspd);
        ctxt->track_item_to_release(si_result);

        si_result->set_binded_context(ctxt);
        return si_result;
    }
} simple_service_interaction;

TEST(async_rpc, should_support__simple__async__service_interaction____as_service_B) {
    RequestFoo req_from_a;
    req_from_a.fooa = 100;

    msgrpc::RpcCell<ResponseBar>* rsp = simple_service_interaction.run(req_from_a);

    auto derivedAction = msgrpc::derive_action(
        [](msgrpc::RpcCell<ResponseBar>* r) -> void {
            cout << "----> send data back to original requseter." << endl;
        }, rsp
    );

    msgrpc::RspMsgHeader h;
    ResponseBar bar;
    bar.bara = 101;
    uint8_t* pbuf;
    uint32_t len;
    if (!ThriftEncoder::encode(bar, &pbuf, &len)) {
        cout << "encode failed." << endl;
        return;
    }
    rspc->set_rpc_rsp(&h, (const char*)pbuf, len);

    ResponseBar bar2;
    bar2.bara = 103;
    uint8_t* pbuf2;
    uint32_t len2;
    if (!ThriftEncoder::encode(bar2, &pbuf2, &len2)) {
        cout << "encode failed." << endl;
        return;
    }

    rspd->set_rpc_rsp(&h, (const char*)pbuf2, len2);
};


////////////////////////////////////////////////////////////////////////////////
//TODO: define following macros:
#define declare_interface_on_consumer
#define  define_interface_on_consumer
#define declare_interface_on_provider
#define  define_interface_on_provider

#define define_mock_rpc_interface_provider as_following:
struct MockIBuzzMathImpl : msgrpc::InterfaceImplBaseT<MockIBuzzMathImpl, 1> {
    bool negative_fields(const RequestFoo &req, ResponseBar &rsp) {return true; }
    bool plus1_to_fields(const RequestFoo &req, ResponseBar &rsp) {return false;}
};


////////////////////////////////////////////////////////////////////////////////
//---------------- generate this part by macros set:
struct IBuzzMathImpl : msgrpc::InterfaceImplBaseT<IBuzzMathImpl, 1> {
    bool negative_fields(const RequestFoo& req, ResponseBar& rsp);
    bool plus1_to_fields(const RequestFoo& req, ResponseBar& rsp);

    virtual bool onRpcInvoke( const msgrpc::ReqMsgHeader& msg_header
            , const char* msg, size_t len
            , msgrpc::RspMsgHeader& rsp_header
            , uint8_t*& pout_buf, uint32_t& out_buf_len) override;
};

////////////////////////////////////////////////////////////////////////////////
//---------------- generate this part by macros set: interface_implement_define.h
IBuzzMathImpl buzzMath;

bool IBuzzMathImpl::onRpcInvoke( const msgrpc::ReqMsgHeader& req_header, const char* msg
        , size_t len, msgrpc::RspMsgHeader& rsp_header
        , uint8_t*& pout_buf, uint32_t& out_buf_len) {
    bool ret;

    if (req_header.method_index_in_interface_ == 1) {
        ret = this->invoke_templated_method(&IBuzzMathImpl::negative_fields, msg, len, pout_buf, out_buf_len);
    } else

    if (req_header.method_index_in_interface_ == 2) {
        ret = this->invoke_templated_method(&IBuzzMathImpl::plus1_to_fields, msg, len, pout_buf, out_buf_len);
    } else

    {
        rsp_header.rpc_result_ = msgrpc::RpcResult::method_not_found;
        return false;
    }

    if (! ret) {
        rsp_header.rpc_result_ = msgrpc::RpcResult::failed;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
//---------------- implement interface in here:
bool IBuzzMathImpl::negative_fields(const RequestFoo& req, ResponseBar& rsp) {
    rsp.__set_bara(req.get_foob());
    if (req.__isset.foob) {
        rsp.__set_barb(req.fooa);
    }
    return true;
}

bool IBuzzMathImpl::plus1_to_fields(const RequestFoo& req, ResponseBar& rsp) {
    rsp.__set_bara(1 + req.fooa);
    if (req.__isset.foob) {
        rsp.__set_barb(1 + req.get_foob());
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//-----------generate by:  declare and define stub macros
struct IBuzzMathStub : msgrpc::RpcStubBase {
    virtual msgrpc::RpcRspCell<ResponseBar>* negative_fields(const RequestFoo&);
    virtual msgrpc::RpcRspCell<ResponseBar>* plus1_to_fields(const RequestFoo&);
};

msgrpc::RpcRspCell<ResponseBar>* IBuzzMathStub::negative_fields(const RequestFoo& req) {
    return encode_request_and_send<RequestFoo, ResponseBar>(1, 1, req);
}

msgrpc::RpcRspCell<ResponseBar>* IBuzzMathStub::plus1_to_fields(const RequestFoo& req) {
    return encode_request_and_send<RequestFoo, ResponseBar>(1, 2, req);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {
    template<typename T, typename U>
    struct MsgRpcSIBase { /*SI is short for service interaction*/
        msgrpc::RpcCell<U> *run(const T &req) {
            msgrpc::RpcContext *ctxt = new msgrpc::RpcContext();

            msgrpc::RpcCell<U> *result_cell = do_run(req, ctxt);
            result_cell->set_binded_context(ctxt);

            return result_cell;
        }

        virtual msgrpc::RpcCell<U> *do_run(const T &req, msgrpc::RpcContext *ctxt) = 0;
    };
}

void save_rsp_from_other_services_to_db(msgrpc::RpcCell<ResponseBar> *r) { cout << "1 ----------------->>>> write db." << endl; };
void save_rsp_to_log(msgrpc::RpcCell<ResponseBar> *r) { cout << "2 ----------------->>>> save_log." << endl; };

struct SimpleMsgRpcSI : msgrpc::MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual msgrpc::RpcCell<ResponseBar>* do_run(const RequestFoo &req, msgrpc::RpcContext *ctxt) override {

        msgrpc::RpcRspCell<ResponseBar>* rsp_cell = IBuzzMathStub().negative_fields(req);
        ctxt->track_item_to_release(rsp_cell);

        ctxt->track_item_to_release(msgrpc::derive_action(save_rsp_from_other_services_to_db, rsp_cell));


        ctxt->track_item_to_release(msgrpc::derive_action(save_rsp_to_log, rsp_cell));


        return rsp_cell;
    }
} simple_rpc_service_interaction;

void init_rpc() {
    RequestFoo foo; foo.fooa = 97; foo.__set_foob(98);

    //TODO: check nullptr
    msgrpc::RpcCell<ResponseBar>* rsp_cell = simple_rpc_service_interaction.run(foo);

    if (rsp_cell != nullptr) {
        auto derivedAction = derive_final_action(
                [](msgrpc::RpcCell<ResponseBar> *r) -> void {
                    cout << "final ----------------->>>> send data back to original requseter." << endl;
                    UdpChannel::close_all_channels();
                }, rsp_cell
        );
    }
}

void local_service() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    msgrpc::Config::instance().init_with(&UdpMsgChannel::instance(), k_msgrpc_request_msg_id, k_msgrpc_response_msg_id);

    UdpChannel channel(k_local_service_id,
        [](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
               return init_rpc();
            }

            if (msg_id == msgrpc::Config::instance().response_msg_id_) {
               return msgrpc::RpcRspDispatcher::instance().handle_rpc_rsp(msg_id, msg, len);
            }

            cout << "local received msg:" << string(msg, len) << endl;
        }
    );
}

////////////////////////////////////////////////////////////////////////////////
void remote_service() {
    msgrpc::Config::instance().init_with(&UdpMsgChannel::instance(), k_msgrpc_request_msg_id, k_msgrpc_response_msg_id);

    UdpChannel channel(k_remote_service_id,
        [](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
           if (0 == strcmp(msg, "init")) {
               return;
           }

           msgrpc::RpcReqMsgHandler::on_rpc_req_msg(msg_id, msg, len);
        }
    );
}

////////////////////////////////////////////////////////////////////////////////
TEST(async_rpc, should_able_to__support_async_rpc) {
    std::thread local_thread(local_service);
    std::thread remote_thread(remote_service);

    local_thread.join();
    remote_thread.join();
}
