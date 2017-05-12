#ifndef MSGRPC_SINGLETON_H
#define MSGRPC_SINGLETON_H

namespace msgrpc {

    template<typename T>
    struct ThreadLocalSingleton {
        virtual ~ThreadLocalSingleton() = default;

        static T &instance() {
            static thread_local T t;
            return t;
        }

    protected:
        ThreadLocalSingleton() {}
        ThreadLocalSingleton(ThreadLocalSingleton const&) = delete;
        void operator=(ThreadLocalSingleton const&)       = delete;
    };


    template<typename T>
    struct Singleton {
        virtual ~Singleton() = default;

        static T& instance() {
            static T t;
            return t;
        }

    protected:
        Singleton() {}
        Singleton(Singleton const&)      = delete;
        void operator=(Singleton const&) = delete;
    };
}

#endif //MSGRPC_SINGLETON_H
