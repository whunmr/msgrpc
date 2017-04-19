#ifndef MSGRPC_IFACE_IMPL_UTILITY_H
#define MSGRPC_IFACE_IMPL_UTILITY_H

#include <msgrpc/core/cell/cell.h>

namespace msgrpc {
    template<typename RSP>
    msgrpc::Cell<RSP>* call_sync_iface_impl(std::function<void(RSP &)> f) {
        auto* rsp_cell = new msgrpc::Cell<RSP>();

        RSP rsp;
        f(rsp);

        rsp_cell->set_value(rsp);
        return rsp_cell;
    }

    template<typename T>
    Cell<T>* failed_cell_with_reason(RpcContext &ctxt, const RpcResult& failed_reason) {
        Cell<T>* cell = Cell<T>::new_instance(failed_reason);
        return ctxt.track(cell);
    }
}

#endif //MSGRPC_IFACE_IMPL_UTILITY_H
