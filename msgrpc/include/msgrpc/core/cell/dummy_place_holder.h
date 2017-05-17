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

#endif //PROJECT_DUMMY_PLACE_HOLDER_H
