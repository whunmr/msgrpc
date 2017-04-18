#ifndef MSGRPC_DERIVED_ASYNC_CELL_H
#define MSGRPC_DERIVED_ASYNC_CELL_H

#include <msgrpc/core/cell/cell.h>

namespace msgrpc {

    template<typename T, typename... Args>
    struct DerivedAsyncCell : Cell<T> {
        DerivedAsyncCell(RpcContext& ctxt, std::function<Cell<T>&(void)> f, Args&&... args)
        : ctxt_(ctxt), f_(f) {
            Cell<T>::register_as_listener(std::forward<Args>(args)...);
        }

        void update() override {
            if (!CellBase<T>::has_value_) {  //TODO:refactor to got_rsp, which can be got_value or got_error

                Cell<T>& cell = f_();
                if (cell.has_error()) {
                    this->Cell<T>::set_failed_reason(cell.failed_reason());
                } else {
                    derive_action( ctxt_
                            , [this](const Cell<T>& rsp) {
                                if (rsp.has_error()) {
                                    this->Cell<T>::set_failed_reason(rsp.failed_reason());
                                } else {
                                    this->Cell<T>::set_cell_value(rsp);
                                }
                            }
                            , &cell);
                }
            }
        }

        RpcContext& ctxt_;
        std::function<Cell<T>&(void)> f_;
    };

    template<typename F, typename... Args>
    auto derive_async_cell(RpcContext& ctxt, F f, Args &&... args) -> DerivedAsyncCell<typename std::remove_reference<typename std::result_of<F()>::type>::type::value_type, decltype(*args)...>* {
        auto cell = new DerivedAsyncCell<typename std::remove_reference<typename std::result_of<F()>::type>::type::value_type, decltype(*args)...>(ctxt, f, std::ref(*args)...);
        ctxt.track(cell);
        return cell;
    }

}

#define ___async_cell(logic, ...) \
        derive_async_cell(ctxt , [&ctxt, __VA_ARGS__]() -> Cell<ResponseBar>& { return logic(ctxt, __VA_ARGS__); } , __VA_ARGS__);

#endif //MSGRPC_DERIVED_ASYNC_CELL_H
