#ifndef MSGRPC_SINGLETON_H
#define MSGRPC_SINGLETON_H

namespace msgrpc {

    template<typename T>
    struct Singleton {
        virtual ~Singleton() {}

        static T &instance() {
            static thread_local T t;
            return t;
        }
    };

}

#endif //MSGRPC_SINGLETON_H
