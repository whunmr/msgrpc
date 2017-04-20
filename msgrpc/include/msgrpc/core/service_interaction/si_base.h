#ifndef MSGRPC_SI_BASE_H
#define MSGRPC_SI_BASE_H

#include <msgrpc/core/cell/cell.h>

namespace msgrpc {

    template<typename T, typename U>
    struct SIBase { /*SI is short for service interaction*/
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

#define DEFINE_SI(SI_NAME_, REQ_, RSP_)                                                         \
    struct SI_NAME_ : msgrpc::SIBase<REQ_, RSP_> {                                              \
        virtual msgrpc::Cell<RSP_>* do_run(const REQ_ &req, msgrpc::RpcContext& ctxt) override; \
    };                                                                                          \
    msgrpc::Cell<RSP_>* SI_NAME_::do_run(const REQ_ &req, msgrpc::RpcContext& ctxt)


#endif //MSGRPC_SI_BASE_H
