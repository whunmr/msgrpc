////////////////////////////////////////////////////////////////////////////////
#ifdef ___as_struct
#undef ___as_struct
#endif

#define ___as_struct(...)


#ifndef PROJECT_API_INTERFACE_STUB_DEFINE_H
#define PROJECT_API_INTERFACE_STUB_DEFINE_H

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
#define ___expand_define_interface_stub_method(method_index_, rsp_, method_name_, req_, comments_, iface_name_, iface_id_, ...) \
    msgrpc::Cell<rsp_>* iface_name_::method_name_(const req_& req) {               \
        return encode_request_and_send<req_, rsp_>(g_service_name, iface_id_, method_index_, req); \
    }

////////////////////////////////////////////////////////////////////////////////
//TODO: add header of IfaceStubBase as common declaration include
#define ___define_interface_stub_methods(iface_name_, iface_id_) \
            ___apply_expand(iface_name_, ___expand_define_interface_stub_method, iface_name_, iface_id_)


#define ___define_interface_stub(...)                       \
            ___define_interface_stub_methods(__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
#ifdef ___as_interface
#undef ___as_interface
#endif

#define ___as_interface(...) \
            ___define_interface_stub(__VA_ARGS__)


#endif //PROJECT_API_INTERFACE_STUB_DEFINE_H
