#ifndef PROJECT_UPDATABLE_H
#define PROJECT_UPDATABLE_H

namespace msgrpc {
    struct Updatable {
        virtual ~Updatable() {}
        virtual void update() = 0;
    };
}

#endif //PROJECT_UPDATABLE_H
