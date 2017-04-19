#ifndef MSGRPC_TIMEOUT_CELL_H
#define MSGRPC_TIMEOUT_CELL_H

#include <msgrpc/core/cell/cell.h>

namespace msgrpc {

    template<typename T, typename... Args>
    struct TimeoutCell : Cell<T> {
        TimeoutCell(RpcContext& ctxt, timeout_len_t timeout_ms, size_t retry_times, std::function<Cell<T>* (Args...)> f, Args&&... args)
                : ctxt_(ctxt), timeout_ms_(timeout_ms), retry_times_(retry_times), f_(f), bind_(f, args...) {
            if (sizeof...(args) == 0) {
                invoke_rpc_once(timeout_ms);
            } else {
                Cell<T>::register_as_listener(std::forward<Args>(args)...);
            }
        }

        ~TimeoutCell() {
            if (timeout_cell_ != nullptr) {
                delete timeout_cell_;
                timeout_cell_ = nullptr;
            }
        }

        void invoke_rpc_once(timeout_len_t timeout_ms) {
            Cell<T>* rpc_cell_ = bind_();
            if (rpc_cell_ == nullptr) {
                //init rpc is deferred, because trigger cells are not ready to continue;
                return;
            }

            is_rpc_started_ = true;

            if (rpc_cell_->has_error()) {
                return this->set_failed_reason(rpc_cell_->failed_reason());
            }

            assert(rpc_cell_->has_seq_id_ && "only support add timer guard on cells with seq_id.");
            msgrpc::Config::instance().timer_->set_timer(timeout_ms, Config::instance().set_timer_msg_id_, reinterpret_cast<void*>(rpc_cell_->seq_id_));
            bind_timeout_cell(*rpc_cell_);
        }

        void bind_timeout_cell(Cell<T>& cell_to_bind) {
            derive_action( ctxt_
                    , [this](const Cell<T>& rsp) {
                        if (rsp.is_timeout()) {
                            retry_rpc_if_need(rsp);
                        } else {
                            if (rsp.has_value_or_error()) {
                                assert(rsp.has_seq_id_ && "only support add timer guard on cells with seq_id.");
                                msgrpc::Config::instance().timer_->cancel_timer(Config::instance().set_timer_msg_id_, reinterpret_cast<void*>(rsp.seq_id_));
                            }
                            this->set_cell_value(rsp);
                        }
                    }
                    , &cell_to_bind);
        }

        void retry_rpc_if_need(const Cell<T> &rsp) {
            if (retry_times_ > 0) {
                --retry_times_;
                invoke_rpc_once(timeout_ms_);
            } else {
                this->set_failed_reason(rsp.failed_reason());

                if (timeout_cell_ != nullptr) {
                    timeout_cell_->set_value(true);
                }
            }
        }

        void update() override {
            assert(sizeof...(Args) != 0 && "should not call update if this cell do not dependent other cells.");

            //TODO: check status of both trigger cells
            //TODO: set rpc_has_started_ to true, after bind_() invoked
            bool should_start_rpc = !CellBase<T>::has_value_or_error() && !is_rpc_started_;
            if (should_start_rpc) {
                invoke_rpc_once(timeout_ms_);
            }
        }

        CellBase<bool>* timeout() {
            if (timeout_cell_ == nullptr) {
                timeout_cell_ = new CellBase<bool>;
            }

            return timeout_cell_;
        }

        CellBase<bool>* timeout_cell_ = {nullptr};

        RpcContext& ctxt_;
        timeout_len_t timeout_ms_;
        size_t retry_times_;
        std::function<Cell<T>* (Args...)> f_;

        bool is_rpc_started_ = {false};

        using bind_type = decltype(std::bind(std::declval<std::function<Cell<T>* (Args...)>>(), std::ref(std::declval<Args>())...));
        bind_type bind_;
    };

    template<typename F, typename... Args>
    auto derive_rpc_cell(RpcContext &ctxt, timeout_len_t timeout_ms, size_t retry_times, F f, Args &&... args)
    -> TimeoutCell<typename std::remove_pointer<typename std::result_of<F(decltype(*args)...)>::type>::type::value_type, decltype(*args)...>* {
        auto cell = new TimeoutCell<typename std::remove_pointer<typename std::result_of<F(decltype(*args)...)>::type>::type::value_type, decltype(*args)...>(ctxt, timeout_ms, retry_times, f, std::ref(*args)...);
        ctxt.track(cell);
        return cell;
    }

    template<typename F, typename... Args>
    auto derive_rpc_cell(RpcContext &ctxt, timeout_len_t timeout_ms, F f, Args &&... args)
    -> TimeoutCell<typename std::remove_pointer<typename std::result_of<F(decltype(*args)...)>::type>::type::value_type, decltype(*args)...>* {
        auto cell = new TimeoutCell<typename std::remove_pointer<typename std::result_of<F(decltype(*args)...)>::type>::type::value_type, decltype(*args)...>(ctxt, timeout_ms, /*retry_times=*/0, f, std::ref(*args)...);
        ctxt.track(cell);
        return cell;
    }
}

#define ___ms(...) __VA_ARGS__

#define ___retry(...) __VA_ARGS__

#define ___rpc(...) derive_rpc_cell(ctxt, __VA_ARGS__)

#endif //MSGRPC_TIMEOUT_CELL_H
