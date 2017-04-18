#ifndef MSGRPC_DERIVED_ACTION_H
#define MSGRPC_DERIVED_ACTION_H

#include <msgrpc/core/cell/cell.h>

namespace msgrpc {

    template<typename T, typename... Args>
    struct DerivedAction : Updatable {
        DerivedAction(bool is_final_action, std::function<T(Args...)> logic, Args &&... args)
                : is_final_action_(is_final_action), bind_(logic, std::ref(args)...) {
            register_as_listener(std::forward<Args>(args)...);
        }

        ~DerivedAction() {
            cell_ = nullptr;
        }

        template<typename C, typename... Ts>
        void register_as_listener(C &&c, Ts &&... args) {
            c.register_listener(this);
            register_as_listener(std::forward<Ts>(args)...);
        }

        template<typename C>
        void register_as_listener(C &&c) {
            c.register_listener(this);

            if (is_final_action_) {
                assert(c.context_ != nullptr && "final action should bind to cell with resouce management context.");
                c.context_->track(this);
                cell_ = &c;
            }
        }

        void update() override {
            if (! is_final_action_) {
                return bind_();  //if not final action, may trigger self's destruction, can not continue running.
            }

            bind_();
            if (is_final_action_) {
                delete cell_;
            }
        }

        bool is_final_action_ = {false};
        CellStatus* cell_ = {nullptr};
        using bind_type = decltype(std::bind(std::declval<std::function<T(Args...)>>(), std::ref(std::declval<Args>())...));
        bind_type bind_;
    };

    template<typename F, typename... Args>
    auto derive_action(RpcContext& ctxt, F &&f, Args &&... args) -> DerivedAction<decltype(f(*args...)), decltype(*args)...>* {
        auto action = new DerivedAction<decltype(f(*args...)), decltype(*args)...>(/*is_final_action=*/false, std::forward<F>(f), std::ref(*args)...);
        ctxt.track(action);
        return action;
    }

    template<typename F, typename... Args>
    auto derive_final_action(F &&f, Args &&... args) -> DerivedAction<decltype(f(*args...)), decltype(*args)...>* {
        return new DerivedAction<decltype(f(*args...)), decltype(*args)...>(/*is_final_action=*/true, std::forward<F>(f), std::ref(*args)...);
    }

}

#define ___action(action, ...) derive_action(ctxt, action, __VA_ARGS__);

#endif //MSGRPC_DERIVED_ACTION_H
