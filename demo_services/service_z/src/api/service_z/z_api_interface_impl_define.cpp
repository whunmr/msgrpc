#include <api/service_z/z_api_interface_declare.h>

#include <msgrpc/api/api_interface_impl_define.h>
#include <api/service_z/service_z.api>

#include <msgrpc/core/service_interaction/si_base.h>
#include <api/service_k/k_api_interface_declare.h>
#include <msgrpc/core/cell/timeout_cell.h>
#include <msgrpc/core/cell/derived_cell.h>

namespace service_z {
    using namespace msgrpc;
    using namespace service_k;

    Cell<ZRsp>* IZ_impl::___z_f1m1(const ZReq& req) {
        std::cout << "[DEBUG] got request from service_y" << std::endl;
        ZRsp rsp;
        rsp.zrspa = 7;

        return cell_with_value(rsp);
    }

    void map_krsp_to_zrsp(Cell<ZRsp>& result, Cell<KRsp>& krsp)  {
        if (krsp.has_value()) {
            ZRsp zrsp;
            zrsp.__set_zrspa(krsp.value().krspa * 2);
            result.set_value(zrsp);
        } else {
            result.set_failed_reason(krsp.failed_reason());
        }
    };


    msgrpc::Cell<ZRsp>* SI_call_k_async(const ZReq& req, msgrpc::RpcContext& ctxt) {
        auto call_k_f1m1 = [&ctxt, req]() {
            service_k::KReq kreq;
            return service_k::IK(ctxt).___k_f1m1(kreq);
        };

        auto ___1 = ___rpc(___ms(5000), call_k_f1m1);
        auto ___2 = ___cell(map_krsp_to_zrsp) <------ ___on(___1);

        return ___2;
    }

    Cell<ZRsp>* IZ_impl::___z_async_f1m1(const ZReq& req) {
        return run_si(SI_call_k_async, req);
    }


} //namespace service_y