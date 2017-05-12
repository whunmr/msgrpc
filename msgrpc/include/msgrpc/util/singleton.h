#ifndef MSGRPC_SINGLETON_H
#define MSGRPC_SINGLETON_H

namespace msgrpc {

    template<typename T>
    struct ThreadLocalSingleton {
        ThreadLocalSingleton(ThreadLocalSingleton const&) = delete;
        void operator=(ThreadLocalSingleton const&)       = delete;
        virtual ~ThreadLocalSingleton() = default;

        static T &instance() {
            static thread_local T t;
            return t;
        }

      protected:
        ThreadLocalSingleton() = default;
    };

    ////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    struct Singleton {
        Singleton(Singleton const&)      = delete;
        void operator=(Singleton const&) = delete;
        virtual ~Singleton() = default;

        static T& instance(){
            static T t;
            return t;
        }

      protected:
        Singleton() = default;
    };
}

#endif //MSGRPC_SINGLETON_H
