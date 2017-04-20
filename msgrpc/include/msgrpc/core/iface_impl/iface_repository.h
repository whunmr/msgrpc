#ifndef MSGRPC_IFACE_REPOSITORY_H
#define MSGRPC_IFACE_REPOSITORY_H

#include <cassert>
#include <map>

#include <msgrpc/util/singleton.h>
#include <msgrpc/core/types.h>
#include <msgrpc/core/iface_impl/iface_impl_base.h>

namespace msgrpc {

    struct IfaceRepository : msgrpc::Singleton<IfaceRepository> {
        void add_iface_impl(iface_index_t ii, IfaceImplBase* iface) {
            assert(iface != nullptr && "interface_impl implementation can not be null");
            assert(___m.find(ii) == ___m.end() && "interface_impl can only register once");
            ___m[ii] = iface;
        }

        IfaceImplBase* get_iface_impl_by(iface_index_t ii) {
            auto iter = ___m.find(ii);
            return iter == ___m.end() ? nullptr : iter->second;
        }

    private:
        std::map<iface_index_t, IfaceImplBase*> ___m;
    };

}

#endif //MSGRPC_IFACE_REPOSITORY_H
