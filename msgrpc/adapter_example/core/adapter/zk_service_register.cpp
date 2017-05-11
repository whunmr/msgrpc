#include <adapter_example/core/adapter/zk_service_register.h>

namespace demo {
    vector<string> ZkServiceRegister::old_services_;   //owned by zk client thread
}
