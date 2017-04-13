#include <iostream>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <msgrpc/thrift_struct/thrift_codec.h>
#include <msgrpc/frp/cell.h>
#include <type_traits>
#include <future>
#include <atomic>
#include <msgrpc/util/type_traits.h>

using namespace std;
using namespace std::chrono;

#include "demo/demo_api_declare.h"

//TODO: make long long as timeout_len_t for timer funcs
////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {
    template <typename T> struct Ret {};

    typedef unsigned short msg_id_t;
    typedef unsigned short service_id_t; //TODO: how to deal with different service id types

    struct MsgChannel {
        //TODO: extract common channel interface
        //TODO: check common return value type
        virtual bool send_msg(const service_id_t& remote_service_id, msg_id_t msg_id, const char* buf, size_t len) const = 0;
    };

    struct Config {
        void init_with(MsgChannel *msg_channel, msgrpc::msg_id_t request_msg_id, msgrpc::msg_id_t response_msg_id, msgrpc::msg_id_t set_timer_msg_id, msgrpc::msg_id_t timeout_msg_id) {
            msg_channel_ = msg_channel;
            request_msg_id_  = request_msg_id;
            response_msg_id_ = response_msg_id;
            set_timer_msg_id_ = set_timer_msg_id;
            timeout_msg_id_ = timeout_msg_id;
        }

        static inline Config& instance() {
            static thread_local Config instance;
            return instance;
        }

        MsgChannel* msg_channel_ = {nullptr};
        msg_id_t request_msg_id_ = 0;
        msg_id_t response_msg_id_ = 0;
        msg_id_t set_timer_msg_id_ = 0;
        msg_id_t timeout_msg_id_ = 0;
    };
}

namespace msgrpc {
    const uint32_t k_invalid_sequence_id = 0;

    typedef uint32_t rpc_sequence_id_t;
    struct RpcSequenceId : msgrpc::Singleton<RpcSequenceId> {
        rpc_sequence_id_t get() {
            ++sequence_id_;

            //skip value of k_invalid_sequence_id
            if (sequence_id_ == k_invalid_sequence_id) {
                ++sequence_id_;
            }

            return sequence_id_;
        }

        void reset() {
            sequence_id_ = k_invalid_sequence_id;
        }

    private:
        atomic_uint sequence_id_ = {k_invalid_sequence_id};
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
    };

    /*TODO: consider make msgHeader encoded through thrift*/
    struct ReqMsgHeader : MsgHeader {
    };

    struct RspMsgHeader : MsgHeader {
        RpcResult rpc_result_ = { RpcResult::succeeded };
    };
}

////////////////////////////////////////////////////////////////////////////////
namespace demo {
    struct timer_info {
        long long millionseconds_;
        msgrpc::service_id_t service_id_;
        void *user_data_;
    };

    struct TimerMgr : msgrpc::ThreadLocalSingleton<TimerMgr> {
        void cancel_timer(msgrpc::rpc_sequence_id_t id) {
            canceled_timer_ids_.push_back(id);
        }

        bool should_ignore(const char* msg, size_t len) {
            assert(msg != nullptr && len == sizeof(timer_info));
            timer_info& ti = *(timer_info*)msg;

            msgrpc::rpc_sequence_id_t seq_id =  (msgrpc::rpc_sequence_id_t)((uintptr_t)(ti.user_data_));
            if (std::find(canceled_timer_ids_.begin(), canceled_timer_ids_.end(), seq_id) != canceled_timer_ids_.end()) {
                return true;
            }

            return false;
        }

        void reset() {
            canceled_timer_ids_.clear();
        }

        std::vector<msgrpc::rpc_sequence_id_t> canceled_timer_ids_;
    };
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TODO: extract to set timer interface
void set_timer(long long millionseconds, msgrpc::msg_id_t timeout_msg_id, void* user_data);
void cancel_timer(msgrpc::msg_id_t timeout_msg_id, void* user_data) {
    cout << "cancel timer for timer id: " << (msgrpc::rpc_sequence_id_t)((uintptr_t)(user_data)) << endl;
    demo::TimerMgr::instance().cancel_timer(msgrpc::rpc_sequence_id_t((uintptr_t)user_data));
}

////////////////////////////////////////////////////////////////////////////////
const msgrpc::service_id_t x_service_id = 2222;
const msgrpc::service_id_t y_service_id = 3333;
const msgrpc::service_id_t timer_service_id = 5555;

const msgrpc::msg_id_t k_msgrpc_request_msg_id = 101;
const msgrpc::msg_id_t k_msgrpc_response_msg_id = 102;
const msgrpc::msg_id_t k_msgrpc_set_timer_msg = 103;
const msgrpc::msg_id_t k_msgrpc_timeout_msg = 104;

////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {

    struct RpcRspCellSink {
        virtual void set_rpc_rsp(const RspMsgHeader& rsp_header, const char* msg, size_t len) = 0;
        virtual void set_timeout() = 0;

        virtual void set_sequential_id(const rpc_sequence_id_t& seq_id) = 0;
        virtual void reset_sequential_id() = 0;
    };

    struct RpcRspDispatcher : msgrpc::ThreadLocalSingleton<RpcRspDispatcher> {
        void on_rsp_handler_timeout(rpc_sequence_id_t sequence_id) {
            auto iter = id_func_map_.find(sequence_id);
            if (iter == id_func_map_.end()) {
                cout << "WARNING: not existing handler to remove caused by timeout. seq id: " << sequence_id << endl;
                return;
            }

            RpcRspCellSink* cell = (iter->second);

            cell->reset_sequential_id();
            id_func_map_.erase(iter);

            assert(cell != nullptr && "cell should be notnull when find in dispatch map.");
            cell->set_timeout();
        }

        void remove_rsp_handler(rpc_sequence_id_t sequence_id) {
            auto iter = id_func_map_.find(sequence_id);
            if (iter == id_func_map_.end()) {
                cout << "WARNING: not existing handler to remove: id: " << sequence_id << endl;
                return;
            }

            id_func_map_.erase(iter);
        }

        void register_rsp_Handler(rpc_sequence_id_t sequence_id, RpcRspCellSink* sink) {
            assert(sink != nullptr && "can not register null callback");
            assert(id_func_map_.find(sequence_id) == id_func_map_.end() && "should register with unique id.");
            id_func_map_[sequence_id] = sink;
            sink->set_sequential_id(sequence_id);
        }

        void handle_rpc_rsp(msgrpc::msg_id_t msg_id, const char *msg, size_t len) {
            //cout << "DEBUG: local received msg----------->: " << string(msg, len) << endl;
            if (msg == nullptr) {
                cout << "invalid rpc rsp with msg == nullptr";
                return;
            }

            if (len < sizeof(RspMsgHeader)) {
                cout << "WARNING: invalid rsp msg" << endl;
                return;
            }

            auto* rsp_header = (RspMsgHeader*)msg;

            auto iter = id_func_map_.find(rsp_header->sequence_id_);
            if (iter == id_func_map_.end()) {
                cout << "WARNING: can not find rsp handler" << endl;
                return;
            }

            (iter->second)->set_rpc_rsp(*rsp_header, msg + sizeof(RspMsgHeader), len - sizeof(RspMsgHeader));

            //if this rsp finishes a SI, the handler (iter->second) will be release in whole SI context teardown;
            //otherwise, we should erase this very rsp handler only.
            delete_rsp_handler_if_exist(rsp_header->sequence_id_);
        }

        void delete_rsp_handler_if_exist(const rpc_sequence_id_t& seq_id) {
            auto iter = id_func_map_.find(seq_id);
            if (iter != id_func_map_.end()) {
                (iter->second)->reset_sequential_id();
                id_func_map_.erase(iter);
            }
        }

        std::map<rpc_sequence_id_t, RpcRspCellSink*> id_func_map_;
    };

    struct RspCellBase : RpcRspCellSink {
        virtual ~RspCellBase() {
            if (context_ != nullptr) {
                delete context_;
                context_ = nullptr;
            }

            if (has_seq_id_) {
                RpcRspDispatcher::instance().remove_rsp_handler(seq_id_);
            }
        }

        void set_binded_context(RpcContext& context) {
            context_ = &context;
        }

        virtual void reset_sequential_id() override {
            has_seq_id_ = false;
        }

        virtual void set_sequential_id(const rpc_sequence_id_t& seq_id) override {
            seq_id_ = seq_id;
            has_seq_id_ = true;
        }

        RpcContext* context_ = {nullptr};
        bool has_seq_id_ = false;
        rpc_sequence_id_t seq_id_ = {0};
    };

    template<typename T>
    struct Cell : CellBase<T>, RspCellBase {
        template<typename C, typename... Ts>
        void register_as_listener(C &&c, Ts &&... args) {
            c.register_listener(this);
            register_as_listener(std::forward<Ts>(args)...);
        }

        template<typename C>
        void register_as_listener(C &&c) {
            c.register_listener(this);
        }

        void register_as_listener() {
        }

        virtual void set_rpc_rsp(const RspMsgHeader& rsp_header, const char* msg, size_t len) override {
            if (rsp_header.rpc_result_ != RpcResult::succeeded) {
                cout << "rsp_header->rpc_result_: " << (int)rsp_header.rpc_result_ << endl;
                CellBase<T>::set_failed_reason(rsp_header.rpc_result_);
                return;
            }

            assert((msg != nullptr && len > 0) && "should has payload when rpc succeeded.");

            T rsp;
            if (! ThriftDecoder::decode(rsp, (uint8_t *) msg, len)) {
                cout << "decode rpc response failed. rpc seq_id: [TODO: unique_global_id]" << endl;
                CellBase<T>::set_failed_reason(RpcResult::failed);
                return;
            }

            CellBase<T>::set_value(rsp);
        }

        virtual void set_timeout() override {
            CellBase<T>::set_failed_reason(RpcResult::timeout);
        }

        /*Cell* timeout(long long timeout_len, size_t retry_times, ) {
            //TODO: refactor to extract timer interface
            //static int sequential_num = 22;
            //set_timer(___ms(2000), k_msgrpc_timeout_msg, &sequential_num);
            return this;
        }*/
    };

    template<typename T, typename... Args>
    struct DerivedAction : Updatable {
        DerivedAction(bool is_final_action, std::function<T(Args...)> logic, Args &&... args)
                : is_final_action_(is_final_action), bind_(logic, std::ref(args)...) {
            register_as_listener(std::forward<Args>(args)...);
        }

        ~DerivedAction() {
            cell_ = nullptr;
        }

        template<typename C, typename... Ts>
        void register_as_listener(C &&c, Ts &&... args) {
            c.register_listener(this);
            register_as_listener(std::forward<Ts>(args)...);
        }

        template<typename C>
        void register_as_listener(C &&c) {
            c.register_listener(this);

            if (is_final_action_) {
                assert(c.context_ != nullptr && "final action should bind to cell with resouce management context.");
                c.context_->track(this);
                cell_ = &c;
            }
        }

        void update() override {
            if (! is_final_action_) {
                return bind_();  //if not final action, may trigger self's destruction, can not continue running.
            }

            bind_();
            if (is_final_action_) {
                delete cell_;
            }
        }

        bool is_final_action_ = {false};
        RspCellBase* cell_ = {nullptr};
        using bind_type = decltype(std::bind(std::declval<std::function<T(Args...)>>(), std::ref(std::declval<Args>())...));
        bind_type bind_;
    };

    template<typename F, typename... Args>
    auto derive_action(RpcContext& ctxt, F &&f, Args &&... args) -> DerivedAction<decltype(f(*args...)), decltype(*args)...>* {
        auto action = new DerivedAction<decltype(f(*args...)), decltype(*args)...>(/*is_final_action=*/false, std::forward<F>(f), std::ref(*args)...);
        ctxt.track(action);
        return action;
    }

    template<typename F, typename... Args>
    auto derive_final_action(F &&f, Args &&... args) -> DerivedAction<decltype(f(*args...)), decltype(*args)...>* {
        return new DerivedAction<decltype(f(*args...)), decltype(*args)...>(/*is_final_action=*/true, std::forward<F>(f), std::ref(*args)...);
    }

    template<typename T, typename... Args>
    struct DerivedCell : Cell<T> {
        DerivedCell(std::function<void(Cell<T>&, Args...)> logic, Args&&... args)
                : bind_(logic, std::placeholders::_1, std::ref(args)...) {
            Cell<T>::register_as_listener(std::forward<Args>(args)...);
        }

        void update() override {
            if (!CellBase<T>::has_value_or_error()) {
                bind_(*this);
            }
        }

        using bind_type = decltype(std::bind(std::declval<std::function<void(Cell<T>&, Args...)>>(), std::placeholders::_1, std::ref(std::declval<Args>())...));
        bind_type bind_;
    };

    template<typename F, typename... Args>
    auto derive_cell(RpcContext& ctxt, F f, Args &&... args) -> DerivedCell<typename std::remove_reference<first_argument_type<F>>::type::value_type, decltype(*args)...>* {
        auto cell = new DerivedCell<typename std::remove_reference<first_argument_type<F>>::type::value_type, decltype(*args)...>(f, std::ref(*args)...);
        ctxt.track(cell);
        return cell;
    }


    template<typename T, typename... Args>
    struct DerivedAsyncCell : Cell<T> {
        DerivedAsyncCell(RpcContext& ctxt, std::function<Cell<T>&(void)> f, Args&&... args)
          : ctxt_(ctxt), f_(f) {
            Cell<T>::register_as_listener(std::forward<Args>(args)...);
        }

        void update() override {
            if (!CellBase<T>::has_value_) {  //TODO:refactor to got_rsp, which can be got_value or got_error

                Cell<T>& cell = f_();
                if (cell.has_error()) {
                    this->Cell<T>::set_failed_reason(cell.failed_reason());
                } else {
                    derive_action( ctxt_
                                 , [this](const Cell<T>& rsp) {
                                        if (rsp.has_error()) {
                                            this->Cell<T>::set_failed_reason(rsp.failed_reason());
                                        } else {
                                            this->Cell<T>::set_cell_value(rsp);
                                        }
                                   }
                                 , &cell);
                }
            }
        }

        RpcContext& ctxt_;
        std::function<Cell<T>&(void)> f_;
    };

    template<typename F, typename... Args>
    auto derive_async_cell(RpcContext& ctxt, F f, Args &&... args) -> DerivedAsyncCell<typename std::remove_reference<typename std::result_of<F()>::type>::type::value_type, decltype(*args)...>* {
        auto cell = new DerivedAsyncCell<typename std::remove_reference<typename std::result_of<F()>::type>::type::value_type, decltype(*args)...>(ctxt, f, std::ref(*args)...);
        ctxt.track(cell);
        return cell;
    }


    template<typename T, typename... Args>
    struct TimeoutCell : Cell<T> {
        TimeoutCell(RpcContext& ctxt, long long timeout_ms, size_t retry_times, std::function<Cell<T>* (Args...)> f, Args&&... args)
          : ctxt_(ctxt), timeout_ms_(timeout_ms), retry_times_(retry_times), f_(f), bind_(f, args...) {
            if (sizeof...(args) == 0) {
                invoke_rpc_once(timeout_ms);
            } else {
                Cell<T>::register_as_listener(std::forward<Args>(args)...);
            }
        }

        void invoke_rpc_once(long long int timeout_ms) {
            Cell<T>* rpc_cell_ = bind_();
            if (rpc_cell_ == nullptr) {
                //init rpc is deferred, because trigger cells are not ready to continue;
                return;
            }

            is_rpc_started_ = true;

            if (rpc_cell_->has_error()) {
                return this->set_failed_reason(rpc_cell_->failed_reason());
            }

            set_timer(timeout_ms, Config::instance().set_timer_msg_id_, reinterpret_cast<void*>(rpc_cell_->seq_id_));
            bind_timeout_cell(*rpc_cell_);
        }

        void bind_timeout_cell(Cell<T>& cell_to_bind) {
            derive_action( ctxt_
                         , [this](const Cell<T>& rsp) {
                                if (rsp.is_timeout()) {
                                    retry_rpc_if_need(rsp);
                                } else {
                                    if (rsp.has_value()) {
                                        assert(rsp.has_seq_id_);
                                        cancel_timer(Config::instance().set_timer_msg_id_, reinterpret_cast<void*>(rsp.seq_id_));
                                    }
                                    this->set_cell_value(rsp);
                                }
                           }
                         , &cell_to_bind);
        }

        void retry_rpc_if_need(const Cell<T> &rsp) {
            if (retry_times_ > 0) {
                --retry_times_;
                invoke_rpc_once(timeout_ms_);
            } else {
                this->set_failed_reason(rsp.failed_reason());
            }
        }

        void update() override {
            assert(sizeof...(Args) != 0 && "should not call update if this cell do not dependent other cells.");

            //TODO: check status of both trigger cells
            //TODO: set rpc_has_started_ to true, after bind_() invoked
            bool should_start_rpc = !CellBase<T>::has_value_or_error() && !is_rpc_started_;
            if (should_start_rpc) {
                invoke_rpc_once(timeout_ms_);
            }
        }

        RpcContext& ctxt_;
        long long timeout_ms_;
        size_t retry_times_;
        std::function<Cell<T>* (Args...)> f_;

        bool is_rpc_started_ = {false};

        using bind_type = decltype(std::bind(std::declval<std::function<Cell<T>* (Args...)>>(), std::ref(std::declval<Args>())...));
        bind_type bind_;
    };

    template<typename F, typename... Args>
    auto rpc(RpcContext &ctxt, long long timeout_ms, size_t retry_times, F f, Args &&... args)
      -> TimeoutCell<typename std::remove_pointer<typename std::result_of<F(decltype(*args)...)>::type>::type::value_type, decltype(*args)...>* {
        auto cell = new TimeoutCell<typename std::remove_pointer<typename std::result_of<F(decltype(*args)...)>::type>::type::value_type, decltype(*args)...>(ctxt, timeout_ms, retry_times, f, std::ref(*args)...);
        ctxt.track(cell);
        return cell;
    }
}

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
            if (pout_buf == nullptr || out_buf_len == 0) {
                Config::instance().msg_channel_->send_msg(service_id, k_msgrpc_response_msg_id, (const char*)&rsp_header, sizeof(rsp_header));
                return;
            }

            size_t rsp_len_with_header = sizeof(rsp_header) + out_buf_len;
            char *mem = (char *) malloc(rsp_len_with_header);
            if (mem != nullptr) {
                memcpy(mem, &rsp_header, sizeof(rsp_header));
                memcpy(mem + sizeof(rsp_header), pout_buf, out_buf_len);
                Config::instance().msg_channel_->send_msg(service_id, k_msgrpc_response_msg_id, mem, rsp_len_with_header);
                free(mem);
            }
        }
    };

    struct RpcReqMsgHandler {
        static void on_rpc_req_msg(msgrpc::msg_id_t msg_id, const char *msg, size_t len) {
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

            msgrpc::service_id_t sender_id = req_header->iface_index_in_service_ == 2 ? x_service_id : y_service_id;

            IfaceImplBase *iface = IfaceRepository::instance().get_iface_impl_by(req_header->iface_index_in_service_);
            if (iface == nullptr) {
                rsp_header.rpc_result_ = RpcResult::iface_not_found;
                msgrpc::Config::instance().msg_channel_->send_msg(sender_id, k_msgrpc_response_msg_id, (const char *) &rsp_header, sizeof(rsp_header));
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
    ////////////////////////////////////////////////////////////////////////////////
    enum class MsgRpcRet : unsigned char {
        succeeded = 0,
        failed = 1,
        async_deferred = 2
    };

    template<typename RSP>
    static RpcResult send_rsp_cell_value(const service_id_t& sender_id, const RspMsgHeader &rsp_header, const Cell<RSP>& rsp_cell) {
        if (!rsp_cell.has_value_) {
            return RpcResult::failed;
        }

        uint8_t* pout_buf = nullptr;
        uint32_t out_buf_len = 0;
        if (!ThriftEncoder::encode(rsp_cell.value(), &pout_buf, &out_buf_len)) {
            cout << "encode failed on remtoe side." << endl;
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
                cout << "decode failed on remote side." << endl;
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
            //TODO: find y_service_id by interface name "IBuzzMath"
            msgrpc::service_id_t service_id = iface_index == 1 ? x_service_id : y_service_id;

            bool send_ret = msgrpc::Config::instance().msg_channel_->send_msg(service_id, k_msgrpc_request_msg_id, mem, msg_len_with_header);
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
                cout << "encode failed." << endl;
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
    Cell<T>& failed_cell_with_reason(RpcContext &ctxt, const RpcResult& failed_reason) {
        Cell<T>* cell = new Cell<T>();
        cell->set_failed_reason(failed_reason);
        ctxt.track(cell);
        return *cell;
    }
}

//interface implementation related macros:
#define ___bind_rpc(logic, ...) \
        derive_async_cell( ctxt \
                         , [&ctxt, __VA_ARGS__]() -> Cell<ResponseBar>& { \
                                    return logic(ctxt, __VA_ARGS__); \
                            } \
                         , __VA_ARGS__);


#define ___bind_action(action, ...) derive_action(ctxt, action, __VA_ARGS__);

#define ___bind_cell(logic, ...) derive_cell(ctxt, logic, __VA_ARGS__);

////////////////////////////////////////////////////////////////////////////////
#include "test_util/UdpChannel.h"

namespace demo {

    struct UdpMsgChannel : msgrpc::MsgChannel, msgrpc::ThreadLocalSingleton<UdpMsgChannel> {
        virtual bool send_msg(const msgrpc::service_id_t& remote_service_id, msgrpc::msg_id_t msg_id, const char* buf, size_t len) const {
            if (msg_id != k_msgrpc_set_timer_msg && msg_id != k_msgrpc_timeout_msg) {
                cout << ((remote_service_id == x_service_id) ? "X <------ " : "   ------> Y") << endl;
            }

            size_t msg_len_with_msgid = sizeof(msgrpc::msg_id_t) + len;
            char* mem = (char*)malloc(msg_len_with_msgid);
            if (mem) {
                *(msgrpc::msg_id_t*)(mem) = msg_id;
                memcpy(mem + sizeof(msgrpc::msg_id_t), buf, len);
                g_msg_channel->send_msg_to_remote(string(mem, msg_len_with_msgid), udp::endpoint(udp::v4(), remote_service_id));
                free(mem);
            } else {
                cout << "send msg failed: allocation failure." << endl;
            }
            return true;
        }
    };
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace demo;

//constants for testing.
const int k_req_init_value = 1;
const int k__sync_y__delta = 3;
const int k__sync_x__delta = 17;

struct test_service : msgrpc::ThreadLocalSingleton<test_service> {
    msgrpc::service_id_t current_service_id_;
};

namespace demo {
    struct SetTimerHandler : msgrpc::ThreadLocalSingleton<SetTimerHandler> {
        void set_timer(const char* msg, size_t len) {
            static unsigned short entry_times = 0;
            ++entry_times;

            assert(msg != nullptr && len == sizeof(timer_info));
            timer_info& ti = *(timer_info*)msg;

            unsigned short temp_udp_port = timer_service_id + entry_times;

            std::thread timer_thread([ti, temp_udp_port]{
                msgrpc::Config::instance().init_with(&UdpMsgChannel::instance()
                        , k_msgrpc_request_msg_id
                        , k_msgrpc_response_msg_id
                        , k_msgrpc_set_timer_msg
                        , k_msgrpc_timeout_msg);

                UdpChannel channel(temp_udp_port,
                   [ti](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
                       if (0 == strcmp(msg, "init")) {
                           this_thread::sleep_for(milliseconds(ti.millionseconds_));
                           msgrpc::Config::instance().msg_channel_->send_msg(ti.service_id_, k_msgrpc_timeout_msg, (const char*)&ti, sizeof(ti));
                       }
                   }
                );
            });
            timer_thread.detach();
        }
    };
}

namespace msgrpc {
    struct RpcTimeoutHandler : msgrpc::ThreadLocalSingleton<RpcTimeoutHandler> {
        void on_timeout(const char* msg, size_t len) {
            assert(msg != nullptr && len == sizeof(timer_info));
            timer_info& ti = *(timer_info*)msg;

            rpc_sequence_id_t seq_id =  (rpc_sequence_id_t)((uintptr_t)(ti.user_data_));

            RpcRspDispatcher::instance().on_rsp_handler_timeout(seq_id);
        }
    };
}

void msgrpc_loop(unsigned short udp_port, std::function<void(void)> init_func, std::function<bool(const char* msg, size_t len)> should_drop) {
    msgrpc::Config::instance().init_with(&UdpMsgChannel::instance()
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
                return msgrpc::RpcRspDispatcher::instance().handle_rpc_rsp(msg_id, msg, len);
            } else if (msg_id == msgrpc::Config::instance().set_timer_msg_id_) {
                return demo::SetTimerHandler::instance().set_timer(msg, len);
            } else if (msg_id == msgrpc::Config::instance().timeout_msg_id_) {
                if (! TimerMgr::instance().should_ignore(msg, len)) {
                    return msgrpc::RpcTimeoutHandler::instance().on_timeout(msg, len);
                }
            } else {
                cout << "got unknow msg with id: " << msg_id << endl;
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
    cout << "                     ______sync_x" << endl;
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
    cout << "                     ______sync_y" << endl;
    return msgrpc::call_sync_impl(______sync_y_impl, req);
}


struct SI_____async_y : msgrpc::MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual msgrpc::Cell<ResponseBar>* do_run(const RequestFoo &req, msgrpc::RpcContext& ctxt) override {
        return InterfaceXStub(ctxt).______sync_x(req);
    }
};
msgrpc::Cell<ResponseBar>* InterfaceYImpl::_____async_y(const RequestFoo& req) {
    cout << "                     _____async_y" << endl;
    return SI_____async_y().run(req);
}


msgrpc::Cell<ResponseBar>* InterfaceYImpl::______sync_y_failed(const RequestFoo& req) {
    cout << "                     _____async_y" << endl;
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
    test_thread(Args... args) : std::thread(msgrpc_loop, args...) {
    }

    ~test_thread() {
        join();
    }
};

////////////////////////////////////////////////////////////////////////////////
void create_delayed_exiting_thread() {
    std::thread thread_delayed_exiting(
            []{
                this_thread::sleep_for(milliseconds(1000));

                lock_guard<mutex> lk(can_safely_exit_mutex);
                can_safely_exit = true;
                can_safely_exit_cv.notify_one();

                UdpChannel::close_all_channels();
            });
    thread_delayed_exiting.detach();
}


template<typename SI>
void rpc_main(std::function<void(Cell<ResponseBar>&)> f) {
    this_thread::sleep_for(milliseconds(30));
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
struct SI_case1 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        return InterfaceYStub(ctxt)._____async_y(req);
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc______________case1) {
    // x ----(req1)-------------------------->y  (async_y)
    //        x (sync_x) <=========(req2)=====y  (async_y)
    //        x (sync_x) ==========(rsp2)====>y  (async_y)
    // x <---(rsp1)---------------------------y  (async_y)
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value_);
        EXPECT_EQ(k_req_init_value + k__sync_x__delta, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case1>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void save_rsp_from_other_services_to_db(Cell<ResponseBar>& r) { cout << "1/2 ----------------->>>> write db." << endl; };
void save_rsp_to_log(Cell<ResponseBar>& r)                    { cout << "2/2 ----------------->>>> save_log." << endl; };

struct SI_case2 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
                    ___bind_action(save_rsp_from_other_services_to_db, ___1);
                    ___bind_action(save_rsp_to_log, ___1);
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc______________case2) {
    // x ----(req)---->y (sync_y)
    // x <---(rsp)-----y
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value_);
        EXPECT_EQ(k_req_init_value + k__sync_y__delta, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&] {rpc_main<SI_case2>(then_check);}, not_drop_msg);
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

struct SI_case3 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___2 = InterfaceYStub(ctxt)._____async_y(req);
        return ___bind_cell(merge_logic, ___1, ___2);
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc________parallel_rpc______case3) {
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

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case3>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Cell<ResponseBar>& call__sync_y_again(RpcContext &ctxt, Cell<ResponseBar> *___r) {
    if (___r->has_error()) {
        return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, ___r->failed_reason());
    }

    RequestFoo req;
    req.reqa = ___r->value().rspa;
    return *(InterfaceYStub(ctxt).______sync_y(req)); //TODO: let rpc request return reference to cell
}

struct SI_case4 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
        {
            auto ___2 = ___bind_rpc(call__sync_y_again, ___1);
            {
                return ___bind_rpc(call__sync_y_again, ___2);
            }
        }
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc________sequential_rpc______case4) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value_);
        EXPECT_EQ(k_req_init_value + k__sync_y__delta * 3, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case4_failed : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y_failed(req);
        {
            auto ___2 = ___bind_rpc(call__sync_y_again, ___1);
            {
                return ___bind_rpc(call__sync_y_again, ___2);
            }
        }
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc________sequential_rpc______case4__failure_propagation) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4_failed>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Cell<ResponseBar>& call__sync_y_failed(RpcContext &ctxt, Cell<ResponseBar> *___r) {
    if (___r->has_error()) {
        return msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, ___r->failed_reason());
    }

    RequestFoo req;
    req.reqa = ___r->value().rspa;
    return *(InterfaceYStub(ctxt).______sync_y_failed(req));
}

struct SI_case4_failed_2 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
        {
            auto ___2 = ___bind_rpc(call__sync_y_failed, ___1);
            {
                return ___bind_rpc(call__sync_y_again, ___2);
            }
        }
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc________sequential_rpc______case4__failure_propagation_2) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4_failed_2>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case4_failed_3 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
        {
            auto ___2 = ___bind_rpc(call__sync_y_again, ___1);
            {
                return ___bind_rpc(call__sync_y_failed, ___2);
            }
        }
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc________sequential_rpc______case4__failure_propagation_3) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case4_failed_3>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gen2(Cell<ResponseBar> &result, Cell<ResponseBar> &rsp_cell_1)  {
    if (rsp_cell_1.has_value_) {
        result.set_value(rsp_cell_1);
    }
};

void action1(Cell<ResponseBar> &r) { cout << "1/1 ----------------->>>> action1." << endl; };

struct SI_case5 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___3 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___1 = InterfaceYStub(ctxt)._____async_y(req); ___bind_action(action1, ___1);
        {
            auto ___2 = ___bind_cell(gen2, ___1);
            {
                return ___bind_cell(merge_logic, ___2, ___3);
            }
        }
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc______________case4) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value_);
        int expect_value = (k_req_init_value + k__sync_x__delta) * 2;
        EXPECT_EQ(expect_value, ___r.value().rspa);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case5>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gen5(Cell<ResponseBar>& result, Cell<ResponseBar>& rsp)  {
    if (rsp.has_error()) {
        return result.set_failed_reason(rsp.failed_reason());
    }

    return result.set_value(rsp);
};

struct SI_case6 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo &req, RpcContext& ctxt) override {
        auto ___1 = InterfaceYStub(ctxt).______sync_y_failed(req);
        return ___bind_cell(gen5, ___1);
    }
};

TEST_F(MsgRpcTest, should_able_to__support_simple_async_rpc_______rpc_fails_______case5) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case6>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}, not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TODO: handle timeout and timer
//TODO: add service discovery

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ___ms(...) __VA_ARGS__
#define ___retry(...) __VA_ARGS__

void set_timer(long long millionseconds, msgrpc::msg_id_t timeout_msg_id, void* user_data) {
    msgrpc::service_id_t service_id = test_service::instance().current_service_id_;

    timer_info ti;
    ti.millionseconds_ = millionseconds;
    ti.service_id_ = test_service::instance().current_service_id_;
    ti.user_data_ = user_data;

    Config::instance().msg_channel_->send_msg(timer_service_id, timeout_msg_id, (const char*)&ti, sizeof(ti));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
    define everything as {action}

    action may return cell
    action may return void

    action can be a local, quick, sync, method.
    action can be a remote, slow, async, rpc.
    action can be a SI, which contains lots of actions
    action can be a collection of rpc calls, which are remote, slow, async

    action maybe a timer_guard_action, which has timeout value, retry_times, and retry {action}.

    timeout_rollback_action is also an {action};
    if timeout_rollback_action is an SI action, the timeout SI should running in a new context.
    when timer is out, and timeout {action} start running, the timer gurded cell should filled with error timeout.
#endif

//TODO: invoke timeout handler func when timeout
//TODO: invoke timeout handler SI func when timeout
//TODO: handle concor case: when a detached rpc's cell are not in dependency graph of final result cell,
//      if the result cell finished, can not release response handler of the detached cell.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case7_timeout : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y);
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_rpc_with_timeout_and_retry_______case7_timeout) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case7_timeout>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                        , drop_all_msg);
    test_thread thread_timer(timer_service_id, []{}                                , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//timeout test cases:
//TODO: rpc() when timeout
//TODO: call action when timeout
//TODO: drive cell when timeout

struct SI_case7_timeout_action : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };
        auto do_rpc_rollback = [&ctxt, req](Cell<ResponseBar>& ___1) { cout << "rollback" << endl; return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y);
        auto ___2 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_rollback, ___1);

        return ___2;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_rpc_with_timeout_and_retry_______case7_timeout_action) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case7_timeout_action>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                               , drop_all_msg);
    test_thread thread_timer(timer_service_id, []{}                                       , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case7_cancel_timer_after_success : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y);  //seq_id: 1, 2
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_rpc_with_timeout_and_retry_______case7_cancel_timer_after_success) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case7_cancel_timer_after_success>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                                           , not_drop_msg);
    test_thread thread_timer(timer_service_id, []{}                                                   , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case8 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() { return InterfaceYStub(ctxt).______sync_y(req); };

        auto ___1 = rpc(ctxt, ___ms(100), ___retry(2), do_rpc_sync_y);  //seq_id: 1, 2, 3
        return ___1;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_rpc_with_timeout_and_retry___and_got_result_from_retry_______case8) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_TRUE(___r.has_value_);
        EXPECT_EQ(RpcResult::succeeded, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case8>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                , drop_msg_with_seq_id({1, 2}) );
    test_thread thread_timer(timer_service_id, []{}                        , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case9 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() {
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto do_rpc_sync_y_after_1 = [&ctxt, req](Cell<ResponseBar>& rsp) {
            if (rsp.has_error()) {
                return &msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, rsp.failed_reason());
            }
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto do_rpc_sync_y_after_2 = [&ctxt, req](Cell<ResponseBar>& rsp) {
            if (rsp.has_error()) {
                return &msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, rsp.failed_reason());
            }
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto ___1 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y);
        auto ___2 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y_after_1, ___1);
        auto ___3 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y_after_2, ___2);
        auto ___4 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y_after_2, ___3);
        return ___4;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_rpc_with_timeout_and_retry_______case9) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
        EXPECT_EQ(RpcResult::timeout, ___r.failed_reason());
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case9>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                , drop_all_msg);
    test_thread thread_timer(timer_service_id, []{}                        , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SI_case10 : MsgRpcSIBase<RequestFoo, ResponseBar> {
    virtual Cell<ResponseBar>* do_run(const RequestFoo& req, RpcContext& ctxt) override {
        auto do_rpc_sync_y = [&ctxt, req]() {
            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto do_rpc_sync_y_after_1 = [&ctxt, req](Cell<ResponseBar>& ___1, Cell<ResponseBar>& ___2) -> Cell<demo::ResponseBar>* {
            if (___1.is_empty() || ___2.is_empty()) {
                return nullptr;
            }

            if (___1.has_error() || ___2.has_error()) {
                return &msgrpc::failed_cell_with_reason<ResponseBar>(ctxt, ___1.failed_reason());
            }

            return InterfaceYStub(ctxt).______sync_y(req);
        };

        auto ___1 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y);     //seq_id: 1, 3
        auto ___2 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y);     //seq_id: 2, 4
        auto ___3 = rpc(ctxt, ___ms(100), ___retry(1), do_rpc_sync_y_after_1, ___1, ___2);
        return ___3;
    }
};

TEST_F(MsgRpcTest, should_able_to__support_rpc_with_timeout_and_retry_______case10) {
    auto then_check = [](Cell<ResponseBar>& ___r) {
        EXPECT_FALSE(___r.has_value_);
    };

    test_thread thread_x(x_service_id, [&]{rpc_main<SI_case10>(then_check);}, not_drop_msg);
    test_thread thread_y(y_service_id, []{}                                 , drop_msg_with_seq_id({1, 2, 3, 4}));
    test_thread thread_timer(timer_service_id, []{}                         , not_drop_msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
        //auto ___2 = ___1 --> rpc(ctxt, ___ms(1000), ___retry(2), do_rpc_sync_y);    /*case1*/
        //     ___1 --> if_timeout --> rollback_all_related_commits                     /*case0*/

        //auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
        //auto ___1 = MsgRpcAction<___ms(2000), ___retry(0), timeout_action_func>().run();

        //___1.timeout(___ms(2000), ___retry(0), timeout_action_func);
        //auto ___1 = ___time_guard_action(___ms(2000), ___retry(0), timeout_action_func, InterfaceYStub(ctxt).______sync_y(req));
        //auto ___1 =  struct Action1281 : MsgRpcAction<___ms(2000), ___retry(0), timeout_action_func, InterfaceYStub(ctxt).______sync_y(req) > { Cell* run(){ return nullptr;} }();

        /*auto ___2 = ___action(sync_local_action_foo);
               ___2.timeout(timeout_action_func, ___ms(2000));

        auto ___3 = ___1 --> ___action(SI_foo.run());
             ___3.timeout(timeout_action_func, ___ms(2000));

        auto ___4 = ___3 --> ___action(sync_local_action(func_foo));
             ___4.timeout(timeout_action_func, ___ms(2000));

        auto ___5 = ___4 --> ___action(sync_local_action(func_foo));
             ___5.timeout(timeout_rollback_SI, ___ms(2000));*/

            //___1.bind_timer(timeout_si, ___ms(2000), ___1);
            //___1.bind_timer(timeout_action_func, ___ms(2000), retry(3times));
            //static int sequential_num = 22;
            //set_timer(___ms(2000), k_msgrpc_timeout_msg, &sequential_num);
#endif

#if 0
{
    auto ___1 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(2000), rollback_transaction);
    return ___1;
}

{
    auto ___1 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(2000), retry(3times), rollback_transaction);
    return ___1;
}

#endif
