#ifndef PROJECT_DUMMY_PLACE_HOLDER_H
#define PROJECT_DUMMY_PLACE_HOLDER_H

namespace msgrpc {

    struct DummyPlaceHodler {
        DummyPlaceHodler& operator--() {
            return *this;
        }

        DummyPlaceHodler& operator< (DummyPlaceHodler& p) {
            return p;
        }

    };

    extern struct DummyPlaceHodler g_dummy_holder;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ___on(...) g_dummy_holder, __VA_ARGS__)

#endif //PROJECT_DUMMY_PLACE_HOLDER_H
