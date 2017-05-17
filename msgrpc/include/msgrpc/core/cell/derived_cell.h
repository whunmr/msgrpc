#ifndef MSGRPC_DERIVE_CELL_H
#define MSGRPC_DERIVE_CELL_H

#include <msgrpc/core/cell/cell.h>
#include <msgrpc/util/type_traits.h>
#include <msgrpc/core/cell/dummy_place_holder.h>

namespace msgrpc {

    template<typename T, typename... Args>
    struct DerivedCell : Cell<T> {
        DerivedCell(std::function<void(Cell<T>&, Args...)> logic, Args&&... args)
                : bind_(logic, std::placeholders::_1, std::ref(args)...) {
            Cell<T>::register_as_listener(std::forward<Args>(args)...);
        }

        void update() override {
            if (!CellBase<T>::has_value_or_error()) {
                bind_(*this);
            }
        }

        using bind_type = decltype(std::bind(std::declval<std::function<void(Cell<T>&, Args...)>>(), std::placeholders::_1, std::ref(std::declval<Args>())...));
        bind_type bind_;
    };

    template<typename F, typename... Args>
    auto derive_cell(RpcContext& ctxt, F f, const DummyPlaceHodler& dummyPlaceHodler, Args &&... args) -> DerivedCell<typename std::remove_reference<first_argument_type<F>>::type::value_type, decltype(*args)...>* {
        auto cell = new DerivedCell<typename std::remove_reference<first_argument_type<F>>::type::value_type, decltype(*args)...>(f, std::ref(*args)...);
        ctxt.track(cell);
        return cell;
    }
}

#define ___cell(logic, ...) derive_cell(ctxt, logic, msgrpc::g_dummy_holder


#endif //MSGRPC_DERIVE_CELL_H
