#ifndef PROJECT_INSTANCES_COLLECTOR_H
#define PROJECT_INSTANCES_COLLECTOR_H

#include <msgrpc/util/singleton.h>
#include <list>

namespace msgrpc {

    template<typename T>
    struct InstancesCollector : Singleton<InstancesCollector<T>> {

        void track(T& t) {
            get_list().push_back(&t);
        }

        std::list<T*>& instances() {
            return get_list();
        }

      private:
        std::list<T*>& get_list() {
            static std::list<T*> instances_;
            return instances_;
        }
    };

}

#endif //PROJECT_INSTANCES_COLLECTOR_H
