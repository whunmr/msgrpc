#ifndef PROJECT_SERVICE_API_INTERFACE_DECLARE_H
#define PROJECT_SERVICE_API_INTERFACE_DECLARE_H

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
#define ___declare_interface_impl_class(name_, iface_id_)

////////////////////////////////////////////////////////////////////////////////
#define ___expand_declare_interface_method(method_index_, rsp_, method_name_, req_, comments_, ...) \
    msgrpc::Cell<rsp_>* method_name_(const req_&);

////////////////////////////////////////////////////////////////////////////////
//TODO: add header of IfaceStubBase as common declaration include
#define ___declare_interface_stub_class(iface_name_, iface_id_) \
        struct iface_name_ :  msgrpc::IfaceStubBase { \
            using msgrpc::IfaceStubBase::IfaceStubBase;\
            ___apply_expand(iface_name_, ___expand_declare_interface_method)\
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

#endif //PROJECT_SERVICE_API_INTERFACE_DECLARE_H
