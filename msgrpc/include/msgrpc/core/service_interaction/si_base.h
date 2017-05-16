#ifndef MSGRPC_SI_BASE_H
#define MSGRPC_SI_BASE_H

#include <msgrpc/core/cell/cell.h>
#include <msgrpc/core/service_discovery/default_service_resolver.h>

namespace msgrpc {

    template<typename REQ, typename RSP, typename SERVICE_RESOLVER = DefaultServiceResolver>
    struct SIBase { /*SI is short for service interaction*/
        msgrpc::Cell<RSP> *run(const REQ &req) {
            msgrpc::RpcContext *ctxt = new msgrpc::RpcContext();

            msgrpc::Cell<RSP> *result_cell = do_run(req, *ctxt);

            assert(result_cell != nullptr && "should not return nullptr cell from SI");
            result_cell->set_binded_context(*ctxt);
            ctxt->release_list_.remove(result_cell);

            return result_cell;
        }

        virtual msgrpc::Cell<RSP> *do_run(const REQ &req, msgrpc::RpcContext& ctxt) = 0;
    };
}



#define DEFINE_SI(SI_NAME_, REQ_, RSP_)                                                         \
    struct SI_NAME_ : msgrpc::SIBase<REQ_, RSP_> {                                              \
        virtual msgrpc::Cell<RSP_>* do_run(const REQ_ &req, msgrpc::RpcContext& ctxt) override; \
    };                                                                                          \
    msgrpc::Cell<RSP_>* SI_NAME_::do_run(const REQ_ &req, msgrpc::RpcContext& ctxt)



#define DEFINE_SI_WITH_RESOLVER(SI_NAME_, REQ_, RSP_, SERVICE_RESOLVER)                         \
    struct SI_NAME_ : msgrpc::SIBase<REQ_, RSP_, SERVICE_RESOLVER> {                            \
        virtual msgrpc::Cell<RSP_>* do_run(const REQ_ &req, msgrpc::RpcContext& ctxt) override; \
    };                                                                                          \
    msgrpc::Cell<RSP_>* SI_NAME_::do_run(const REQ_ &req, msgrpc::RpcContext& ctxt)



#endif //MSGRPC_SI_BASE_H
