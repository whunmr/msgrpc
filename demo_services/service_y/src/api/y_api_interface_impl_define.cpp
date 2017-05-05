#include <api/y_api_interface_declare.h>

#include <msgrpc/core/cell/cell.h>
#include <msgrpc/api/api_interface_impl_define.h>
#include <api/service_y.api>

#include <msgrpc/core/service_interaction/si_base.h>
#include <api/z_api_interface_declare.h>
#include <msgrpc/core/cell/timeout_cell.h>
#include <msgrpc/core/cell/derived_cell.h>


namespace service_y {
    using namespace msgrpc;

    Cell<YRsp>* IY_impl::___f1m1(const YReq& req) {

        YRsp rsp;
        rsp.__set_yrspa(req.yreqa * 2);

        return cell_with_value(rsp);
    }

    ////////////////////////////////////////////////////////////////////////////////
    void map_zrsp_to_yrsp(Cell<YRsp>& result, Cell<service_z::ZRsp>& zrsp)  {
        if (zrsp.has_value()) {

            YRsp yrsp;
            yrsp.__set_yrspa(zrsp.value().zrspa);

            result.set_value(yrsp);
        }
    };

    DEFINE_SI(SI_call_z_async, YReq, YRsp) {
        auto call_z_f1m1 = [&ctxt, req]() {
            service_z::ZReq zreq;
            return service_z::IZ(ctxt).___z_f1m1(zreq);
        };

        auto ___1 = ___rpc(___ms(10), call_z_f1m1);
        return ___cell(map_zrsp_to_yrsp, ___1);
    };

    Cell<YRsp>* IY_impl::____async_f1m2(const YReq& req) {
        return SI_call_z_async().run(req);
    }

} //namespace service_y


