#include <iostream>
#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <msgrpc/thrift_struct/thrift_codec.h>
#include <type_traits>
using namespace std;
using namespace std::chrono;

#include "demo/demo_api_declare.h"

#if 0
    #define ___methods_of_interface___IBuzzMath(_, ...)            \
        _(1, ResponseBar, negative_fields, RequestFoo, __VA_ARGS__)\
        _(2, ResponseBar, plus1_to_fields, RequestFoo, __VA_ARGS__)
    ___as_interface(IBuzzMath, __with_interface_id(1))
#endif


////////////////////////////////////////////////////////////////////////////////
namespace msgrpc {
    template <typename T> struct Ret {};

    typedef unsigned short msg_id_t;
    typedef unsigned short service_id_t; //TODO: how to deal with different service id types

    struct MsgChannel {
        virtual uint32_t send_msg(const service_id_t& remote_service_id, msg_id_t msg_id, const char* buf, size_t len) const = 0;
    };

    struct Config {
        void initWith(MsgChannel* msg_channel, msgrpc::msg_id_t request_msg_id, msgrpc::msg_id_t response_msg_id) {
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

    typedef uint8_t  method_index_t;
    typedef uint16_t iface_index_t;

    struct MsgHeader {
        uint8_t           msgrpc_version_ = {0};
        method_index_t    method_index_in_interface_ = {0};
        iface_index_t interface_index_in_service_ = {0};

        //TODO: unsigned char  feature_id_in_service_ = {0};
        //TODO: TLV encoded varient length options
        //unsigned long  sequence_no_;
        //optional 1
        /*TODO: call sequence number*/
    };

    /*TODO: consider make msgHeader encoded through thrift*/
    struct ReqMsgHeader : MsgHeader {
    };

    const unsigned short k_rpc_result_succeeded = 0;
    const unsigned short k_rpc_result_failed = 1;
    const unsigned short k_rpc_result_not_implemented = 2;

    struct RspMsgHeader : MsgHeader {
        unsigned short rpc_result_ = {k_rpc_result_succeeded};
    };
}

////////////////////////////////////////////////////////////////////////////////
#include "test_util/UdpChannel.h"
namespace demo {
    const msgrpc::msg_id_t k_msgrpc_request_msg_id = 101;
    const msgrpc::msg_id_t k_msgrpc_response_msg_id = 102;

    struct UdpMsgChannel : msgrpc::MsgChannel {
        virtual uint32_t send_msg(const msgrpc::service_id_t& remote_service_id, msgrpc::msg_id_t msg_id, const char* buf, size_t len) const {
            size_t msg_len_with_msgid = sizeof(msgrpc::msg_id_t) + len;
            char* mem = (char*)malloc(msg_len_with_msgid);
            if (mem) {
                *(msgrpc::msg_id_t*)(mem) = msg_id;
                memcpy(mem + sizeof(msgrpc::msg_id_t), buf, len);
                cout << "send msg len: " << msg_len_with_msgid << endl;
                g_msg_channel->send_msg_to_remote(string(mem, msg_len_with_msgid), udp::endpoint(udp::v4(), remote_service_id));
                free(mem);
            } else {
                cout << "send msg failed: allocation failure." << endl;
            }
            return 0;
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
using namespace demo;

const msgrpc::service_id_t k_remote_service_id = 2222;
const msgrpc::service_id_t k_local_service_id  = 3333;

struct IBuzzMath {
    virtual msgrpc::Ret<ResponseBar> negative_fields(const RequestFoo&) = 0;
    virtual msgrpc::Ret<ResponseBar> plus1_to_fields(const RequestFoo&) = 0;
};

////////////////////////////////////////////////////////////////////////////////
struct IBuzzMathStub : IBuzzMath {
    virtual msgrpc::Ret<ResponseBar> negative_fields(const RequestFoo&);
    virtual msgrpc::Ret<ResponseBar> plus1_to_fields(const RequestFoo&);
};

msgrpc::Ret<ResponseBar> IBuzzMathStub::negative_fields(const RequestFoo& req) {
    uint8_t* pbuf; uint32_t len;
    /*TODO: extract interface for encode/decode for other protocol adoption such as protobuf*/
    if (!ThriftEncoder::encode(req, &pbuf, &len)) {
        /*TODO: how to do with log, maybe should extract logging interface*/
        cout << "encode failed." << endl;
        return msgrpc::Ret<ResponseBar>();
    }

    //TODO: find k_remote_service_id by interface name "IBuzzMath"
    size_t msg_len_with_header = sizeof(msgrpc::ReqMsgHeader) + len;

    char* mem = (char*)malloc(msg_len_with_header);
    if (!mem) {
        cout << "alloc mem failed, during sending rpc request." << endl;
        return msgrpc::Ret<ResponseBar>();
    }

    auto header = (msgrpc::ReqMsgHeader*)mem;
    header->msgrpc_version_ = 0;
    header->interface_index_in_service_ = 1;
    header->method_index_in_interface_ = 1;
    memcpy(header + 1, (const char*)pbuf, len);

    cout << "stub sending msg with length: " << msg_len_with_header << endl;
    msgrpc::Config::instance().msg_channel_->send_msg(k_remote_service_id, k_msgrpc_request_msg_id, mem, msg_len_with_header);
    free(mem);
    return msgrpc::Ret<ResponseBar>();
}

msgrpc::Ret<ResponseBar> IBuzzMathStub::plus1_to_fields(const RequestFoo& req) {
    return msgrpc::Ret<ResponseBar>();
}

void local_service() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    msgrpc::Config::instance().initWith(new UdpMsgChannel(), k_msgrpc_request_msg_id, k_msgrpc_response_msg_id);

    UdpChannel channel(k_local_service_id,
        [&channel](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                IBuzzMathStub buzzMath;
                RequestFoo foo; foo.fooa = 97; foo.__set_foob(98);
                buzzMath.negative_fields(foo);
            } else {
                cout << "local received msg: " << string(msg, len) << endl;
                channel.close();
            }
        }
    );
}

////////////////////////////////////////////////////////////////////////////////

#include <msgrpc/util/singleton.h>

namespace msgrpc {


    struct IfaceImplBase {
        virtual bool onRpcInvoke(const msgrpc::ReqMsgHeader& msg_header, const char* msg, size_t len, msgrpc::RspMsgHeader& rsp_header, uint8_t*& pout_buf, uint32_t& out_buf_len) = 0;
    };

    struct IfaceRepository : msgrpc::Singleton<IfaceRepository> {
        void add_iface_impl(iface_index_t ii, IfaceImplBase* iface) {
            assert(iface != nullptr && "interface implementation can not be null");
            assert(iface_id_to_ptr_.find(ii) == iface_id_to_ptr_.end() && "interface can only register once");
            iface_id_to_ptr_[ii] = iface;
        }

        IfaceImplBase* get_iface_impl_by(iface_index_t ii) {
            auto iter = iface_id_to_ptr_.find(ii);
            return iter == iface_id_to_ptr_.end() ? nullptr : iter->second;
        }

      private:
        std::map<iface_index_t, IfaceImplBase*> iface_id_to_ptr_;
    };

    template<typename T>
    struct InterfaceImplBaseT : IfaceImplBase {
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



struct IBuzzMathImpl : msgrpc::InterfaceImplBaseT<IBuzzMathImpl> {
    //TODO: 2, auto register to impl repository.

    //TODO: try to unify with stub's signature
    bool negative_fields(const RequestFoo& req, ResponseBar& rsp);
    bool plus1_to_fields(const RequestFoo& req, ResponseBar& rsp);

    virtual bool onRpcInvoke(const msgrpc::ReqMsgHeader& msg_header, const char* msg, size_t len, msgrpc::RspMsgHeader& rsp_header, uint8_t*& pout_buf, uint32_t& out_buf_len) override;
};

bool IBuzzMathImpl::onRpcInvoke(const msgrpc::ReqMsgHeader& msg_header, const char* msg, size_t len, msgrpc::RspMsgHeader& rsp_header, uint8_t*& pout_buf, uint32_t& out_buf_len) {
    cout << "remote receive rpc invoke with: {" << (int)msg_header.msgrpc_version_ << "|" << (int)msg_header.interface_index_in_service_ << "|" << (int)msg_header.method_index_in_interface_ << "}" << endl;
    bool ret = false;

    if (msg_header.method_index_in_interface_ == 1) {
        ret = this->invoke_templated_method(&IBuzzMathImpl::negative_fields, msg, len, pout_buf, out_buf_len);
    } else

    if (msg_header.method_index_in_interface_ == 2) {
        ret = this->invoke_templated_method(&IBuzzMathImpl::plus1_to_fields, msg, len, pout_buf, out_buf_len);
    } else

    {
        rsp_header.rpc_result_ = msgrpc::k_rpc_result_not_implemented;
        return false;
    }

    if (! ret) {
        rsp_header.rpc_result_ = msgrpc::k_rpc_result_failed;
    }

    return true;
}

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
struct RpcReqMsgHandler {
    static void on_rpc_req_msg(msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
        cout << "remote received msg with length: " << len << endl;
        assert(msg_id == k_msgrpc_request_msg_id && "invalid msg id for rpc");

        if (len < sizeof(msgrpc::ReqMsgHeader)) {
            cout << "invalid msg: without sufficient msg header info." << endl;
            return;
        }

        auto* req_header = (msgrpc::ReqMsgHeader*)msg;
        msg += sizeof(msgrpc::ReqMsgHeader);

        msgrpc::RspMsgHeader rsp_header;
        rsp_header.msgrpc_version_ = req_header->msgrpc_version_;
        rsp_header.interface_index_in_service_ = req_header->interface_index_in_service_;
        rsp_header.method_index_in_interface_ = req_header->method_index_in_interface_;

        /*TODO: 1,search interface implementation instance to handle this request*/
        IBuzzMathImpl buzzMath;
        uint8_t* pout_buf; uint32_t out_buf_len;

        if (buzzMath.onRpcInvoke(*req_header, msg, len - sizeof(msgrpc::ReqMsgHeader), rsp_header, pout_buf, out_buf_len)) {
            size_t rsp_len_with_header = sizeof(rsp_header) + out_buf_len;
            char* mem = (char*)malloc(rsp_len_with_header);
            if (mem != nullptr) {
                memcpy(mem, &rsp_header, sizeof(rsp_header));
                memcpy(mem + sizeof(rsp_header), pout_buf, out_buf_len);
                /*TODO: replace k_local_service_id to sender id*/
                msgrpc::Config::instance().msg_channel_->send_msg(k_local_service_id, k_msgrpc_response_msg_id, mem, rsp_len_with_header);
            }
            free(mem);
        } else {
            cout << "not implemented method" << endl; //TODO: using pipelined processor to handling input/output msgheader and rpc statistics.
            msgrpc::Config::instance().msg_channel_->send_msg(k_local_service_id, k_msgrpc_response_msg_id, (const char*)&rsp_header, sizeof(rsp_header));
        }
    }
};


void remote_service() {
    msgrpc::Config::instance().initWith(new UdpMsgChannel(), k_msgrpc_request_msg_id, k_msgrpc_response_msg_id);

    UdpChannel channel(k_remote_service_id,
        [&channel](msgrpc::msg_id_t msg_id, const char* msg, size_t len) {
            if (0 == strcmp(msg, "init")) {
                return;
            }

            RpcReqMsgHandler::on_rpc_req_msg(msg_id, msg, len);
            channel.close();
        }
    );
}

////////////////////////////////////////////////////////////////////////////////
TEST(async_rpc, should_able_to__auto__register_rpc_interface__after__application_startup) {
    demo::RequestFoo req;
    req.fooa = 1;
    req.__set_foob(2);

    std::thread local_thread(local_service);
    std::thread remote_thread(remote_service);

    local_thread.join();
    remote_thread.join();
};
