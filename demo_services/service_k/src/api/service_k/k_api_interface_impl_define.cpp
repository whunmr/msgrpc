#include <api/service_k/k_api_interface_declare.h>

#include <msgrpc/api/api_interface_impl_define.h>
#include <api/service_k/service_k.api>

namespace service_k {
    using namespace msgrpc;

    Cell<KRsp>* IK_impl::___k_f1m1(const KReq& req) {
        std::cout << "[DEBUG] got request from service_k" << std::endl;
        std::cout << req.kreqa << std::endl;
        KRsp rsp;
        rsp.krspa = 2016;
        rsp.__set_krspb(2017);


        return cell_with_value(rsp);
    }

} //namespace service_y