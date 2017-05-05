#include <api/service_z/z_api_interface_declare.h>

#include <msgrpc/core/cell/cell.h>

#include <msgrpc/api/api_interface_impl_define.h>
#include <api/service_z/service_z.api>

namespace service_z {
    using namespace msgrpc;

    Cell<ZRsp>* IZ_impl::___z_f1m1(const ZReq& req) {
        ZRsp rsp;
        rsp.zrspa = 7;

        return cell_with_value(rsp);
    }

} //namespace service_y