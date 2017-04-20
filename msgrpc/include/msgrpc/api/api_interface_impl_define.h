#ifndef PROJECT_API_INTERFACE_IMPL_DEFINE_H
#define PROJECT_API_INTERFACE_IMPL_DEFINE_H

////////////////////////////////////////////////////////////////////////////////
//during interface declare, undef to ignore other macros
#ifdef ___api_version
#undef ___api_version
#endif

#define ___api_version(...)

////////////////////////////////////////////////////////////////////////////////
#ifdef ___as_struct
#undef ___as_struct
#endif

#define ___as_struct(...)

////////////////////////////////////////////////////////////////////////////////
#ifdef ___apply_expand
#undef ___apply_expand
#endif
#define ___apply_expand(iface_name_, field_expand_macro_, ...) \
    ___methods_of_interface___##iface_name_(field_expand_macro_, __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#define ___expand_define_interface_impl_invoke_method(method_index_, rsp_, method_name_, req_, comments_, iface_name_, iface_id_, ...) \
    if (req_header.method_index_in_interface_ == method_index_) { \
        ret = this->invoke_templated_method(&iface_name_##_impl::method_name_, msg, len, sender_id, rsp_header); \
    } else

////////////////////////////////////////////////////////////////////////////////
//TODO: add header of IfaceStubBase as common declaration include
#define ___define_interface_impl_onRpcInvoke_methods(iface_name_, iface_id_)                            \
msgrpc::RpcResult iface_name_##_impl::onRpcInvoke( const msgrpc::ReqMsgHeader& req_header               \
                                                , const char* msg                                       \
                                                , size_t len                                            \
                                                , msgrpc::RspMsgHeader& rsp_header                      \
                                                , msgrpc::service_id_t& sender_id) {                    \
    msgrpc::RpcResult ret;                                                                              \
                                                                                                        \
    ___apply_expand(iface_name_, ___expand_define_interface_impl_invoke_method, iface_name_, iface_id_) \
    {                                                                                                   \
        rsp_header.rpc_result_ = msgrpc::RpcResult::method_not_found;                                   \
        return msgrpc::RpcResult::method_not_found;                                                     \
    }                                                                                                   \
                                                                                                        \
    if (ret == msgrpc::RpcResult::failed) {                                                             \
        rsp_header.rpc_result_ = msgrpc::RpcResult::failed;                                             \
    }                                                                                                   \
    return ret;                                                                                         \
}


#define ___define_interface_impl_global_auto_register_object(iface_name_, iface_id_) \
    iface_name_##_impl iface_name_##_impl##_auto_register_instance;


#define ___define_interface_impl(...) \
            ___define_interface_impl_onRpcInvoke_methods(__VA_ARGS__) \
            ___define_interface_impl_global_auto_register_object(__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#ifdef ___as_interface
#undef ___as_interface
#endif

#define ___as_interface(...) \
            ___define_interface_impl(__VA_ARGS__)

#endif //PROJECT_API_INTERFACE_IMPL_DEFINE_H
