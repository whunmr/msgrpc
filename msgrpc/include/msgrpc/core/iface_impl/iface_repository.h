#ifndef MSGRPC_IFACE_REPOSITORY_H
#define MSGRPC_IFACE_REPOSITORY_H

#include <msgrpc/util/singleton.h>
#include <msgrpc/core/types.h>
#include <msgrpc/core/iface_impl/iface_impl_base.h>

namespace msgrpc {

    struct IfaceRepository : msgrpc::Singleton<IfaceRepository> {

        void add_iface_impl(iface_index_t ii, IfaceImplBase* iface);

        IfaceImplBase* get_iface_impl_by(iface_index_t ii);
    };

}

#endif //MSGRPC_IFACE_REPOSITORY_H
