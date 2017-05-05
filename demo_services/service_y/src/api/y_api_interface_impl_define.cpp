#include <api/y_api_interface_declare.h>

#include <msgrpc/core/cell/cell.h>

#include <msgrpc/api/api_interface_impl_define.h>
#include <api/service_y.api>

namespace service_y {
    using namespace msgrpc;

    Cell<YRsp>* IY_impl::___f1m1(const YReq& req) {

        YRsp rsp;
        rsp.__set_yrspa(req.yreqa * 2);

        return cell_with_value(rsp);
    }




} //namespace service_y