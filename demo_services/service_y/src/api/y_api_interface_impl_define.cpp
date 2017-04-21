#include <api/y_api_interface_declare.h>

#include <msgrpc/api/api_interface_impl_define.h>
#include <api/service_y.api>

namespace service_y {


    std::atomic_uint count {0};

    msgrpc::Cell<YRsp>* IY_impl::___f1m1(const YReq& req) {
        return msgrpc::call_sync_iface_impl<YRsp>(
            [&req](YRsp &rsp) {
                //std::cout << "msgrpc::Cell<YRsp>* IY_impl::___f1m1(const YReq& req)" << std::endl;
                rsp.__set_yrspa(req.yreqa * 2);

                ++count;
                if (count % 100 == 0) {
                    std::cout << count << std::endl;
                }
            }
        );
    }

} //namespace service_y