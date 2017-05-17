#ifndef MSGRPC_SI_BASE_H
#define MSGRPC_SI_BASE_H

#include <msgrpc/core/cell/cell.h>
#include <msgrpc/core/service_discovery/default_service_resolver.h>
#include <msgrpc/util/type_traits.h>

namespace msgrpc {

    template<typename SERVICE_RESOLVER, typename SI_FUNCTION, typename REQ>
    auto run_si(SI_FUNCTION& si_func, const REQ &req) -> function_result_type<SI_FUNCTION> {
        typedef typename std::remove_pointer<function_result_type<SI_FUNCTION>>::type RSP;

        msgrpc::RpcContext *ctxt = new msgrpc::RpcContext();
        ctxt->service_resolver_ = &SERVICE_RESOLVER::instance();

        auto* result_cell = si_func(req, *ctxt);

        assert(result_cell != nullptr && "should not return nullptr cell from SI");
        result_cell->set_binded_context(*ctxt);
        ctxt->release_list_.remove(result_cell);

        return result_cell;
    }

    template<typename SI_FUNCTION, typename REQ>
    auto run_si(SI_FUNCTION& si_func, const REQ &req) -> function_result_type<SI_FUNCTION> {
        return run_si<DefaultServiceResolver>(si_func, req);
    }

}

#endif //MSGRPC_SI_BASE_H
