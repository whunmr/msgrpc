#ifndef MSGRPC_SI_BASE_H
#define MSGRPC_SI_BASE_H

#include <msgrpc/core/cell/cell.h>

namespace msgrpc {

    template<typename T, typename U>
    struct SIBase { /*SI is short for service interaction*/

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

#endif //MSGRPC_SI_BASE_H
