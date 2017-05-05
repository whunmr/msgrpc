////////////////////////////////////////////////////////////////////////////////
#ifdef ___as_struct
#undef ___as_struct
#endif

#define ___as_struct(...)


//#ifndef PROJECT_SERVICE_API_INTERFACE_DECLARE_H
//#define PROJECT_SERVICE_API_INTERFACE_DECLARE_H

////////////////////////////////////////////////////////////////////////////////
//during interface declare, undef to ignore other macros
#ifdef ___def_service
#undef ___def_service
#endif

#define ___def_service(service_name_, version_) namespace service_name_

////////////////////////////////////////////////////////////////////////////////
#ifdef ___apply_expand
#undef ___apply_expand
#endif
#define ___apply_expand(iface_name_, field_expand_macro_, ...) \
    ___methods_of_interface___##iface_name_(field_expand_macro_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___expand_declare_interface_impl_method(method_index_, rsp_, method_name_, req_, comments_, iface_name_, iface_id_) \
    msgrpc::Cell<rsp_>* method_name_(const req_& req);


#define ___declare_interface_impl_class(iface_name_, iface_id_)                                      \
    struct iface_name_##_impl : msgrpc::InterfaceImplBaseT<iface_name_##_impl, iface_id_> {          \
        ___apply_expand(iface_name_, ___expand_declare_interface_impl_method, iface_name_, iface_id_)\
                                                                                                     \
        virtual msgrpc::RpcResult onRpcInvoke( const msgrpc::ReqMsgHeader& msg_header                \
                                             , const char* msg, size_t len                           \
                                             , msgrpc::RspMsgHeader& rsp_header                      \
                                             , msgrpc::service_id_t& sender_id) override;            \
    };


////////////////////////////////////////////////////////////////////////////////
#define ___expand_declare_interface_stub_method(method_index_, rsp_, method_name_, req_, comments_, ...) \
    msgrpc::Cell<rsp_>* method_name_(const req_&);


////////////////////////////////////////////////////////////////////////////////
#define ___declare_interface_stub_class(iface_name_, iface_id_) \
        struct iface_name_ :  msgrpc::IfaceStubBase { \
            using msgrpc::IfaceStubBase::IfaceStubBase;\
            ___apply_expand(iface_name_, ___expand_declare_interface_stub_method)\
        };


#define ___declare_interface(...)                       \
            ___declare_interface_stub_class(__VA_ARGS__)\
            ___declare_interface_impl_class(__VA_ARGS__)


////////////////////////////////////////////////////////////////////////////////
#ifdef ___as_interface
#undef ___as_interface
#endif

#define ___as_interface(...) \
            ___declare_interface(__VA_ARGS__)

//#endif //PROJECT_SERVICE_API_INTERFACE_DECLARE_H
