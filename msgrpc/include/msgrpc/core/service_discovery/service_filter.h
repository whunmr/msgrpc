#ifndef PROJECT_SERVICE_FILTER_H
#define PROJECT_SERVICE_FILTER_H

namespace msgrpc {

    struct ServiceFilter {
        virtual ~ServiceFilter() = default;

        instance_vector_t filter(const instance_vector_t& instances);
    };

}

#endif //PROJECT_SERVICE_FILTER_H
