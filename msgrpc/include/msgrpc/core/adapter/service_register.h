#ifndef PROJECT_SERVICE_REGISTER_H
#define PROJECT_SERVICE_REGISTER_H

namespace msgrpc {

    struct ServiceRegister {
        virtual void register_service(const char* end_point) = 0;
    };

}

#endif //PROJECT_SERVICE_REGISTER_H
