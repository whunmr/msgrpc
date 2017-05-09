#include <msgrpc/core/iface_impl/iface_repository.h>

#include <map>

namespace msgrpc {

    namespace {
        std::map<iface_index_t, IfaceImplBase*> ___m;
    }

    void IfaceRepository::add_iface_impl(iface_index_t ii, IfaceImplBase* iface) {
        assert(iface != nullptr && "interface_impl implementation can not be null");
        assert(___m.find(ii) == ___m.end() && "interface_impl can only register once");
        ___m[ii] = iface;
    }

    IfaceImplBase* IfaceRepository::get_iface_impl_by(iface_index_t ii) {
        auto iter = ___m.find(ii);
        return iter == ___m.end() ? nullptr : iter->second;
    }

}
