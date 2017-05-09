#ifndef PROJECT_ADAPTER_BASE_H
#define PROJECT_ADAPTER_BASE_H

namespace msgrpc {

    struct AdapterBase {
        virtual ~AdapterBase() = default;

        virtual bool init() { return true; }
    };

}

#endif //PROJECT_ADAPTER_BASE_H
