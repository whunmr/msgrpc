#include <api/export/y_api_interface_declare.h>

#include <msgrpc/api/api_interface_impl_define.h>
#include <api/export/service_y.api>

namespace service_y {

    msgrpc::Cell<YRsp>* IY_impl::___f1m1(const YReq& req) {
        return msgrpc::call_sync_iface_impl<YRsp>(
            [&req](YRsp &rsp) {
                rsp.__set_yrspa(req.yreqa * 2);
            }
        );
    }

} //namespace service_y