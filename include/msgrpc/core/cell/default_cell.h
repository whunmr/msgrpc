#ifndef MSGRPC_CELL_GLOBALS_H
#define MSGRPC_CELL_GLOBALS_H

#include <msgrpc/core/cell/cell.h>

namespace msgrpc {

    template<typename T>
    struct DefaultCell {
        static Cell<T>* failed_instance() {
            static Cell<T> t;
            t.status_ = RpcResult::failed;
            return &t;
        }
    };
}

#endif //MSGRPC_CELL_GLOBALS_H
