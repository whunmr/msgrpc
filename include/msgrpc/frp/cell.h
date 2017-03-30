#ifndef MSGRPC_CELL_H
#define MSGRPC_CELL_H
#include <list>
#include <boost/optional.hpp>

#include <iostream> //TODO:remove


//TODO: using std::move during cell value assignment.  by adding trace log in constructor to find out times of copy construction.

namespace msgrpc {
    struct Updatable {
        virtual void update() = 0;
    };

    template<typename T>
    struct Cell {
        bool has_value_{false};
        T value_;

        void set_value(T &&value) {
            std::cout << "cell got value:" << value << std::endl;
            value_ = std::move(value);
            has_value_ = true;

            evaluate_all_derived_cells();
        }

        void evaluate_all_derived_cells() {
            for (auto u : updatables_) {
                if (u != nullptr) {
                    u->update();
                }
            }
        }

        void register_listener(Updatable *updatable) {
            updatables_.push_back(updatable);
        }

        std::list<Updatable *> updatables_;
    };

    template<typename VT, typename... T>
    struct DerivedCell : Cell<VT>, Updatable {
        DerivedCell(std::function<boost::optional<VT>(T...)> logic, T &&... args)
                : bind_(logic, std::forward<T>(args)...) {
            call_each_args(std::forward<T>(args)...);
        }

        template<typename C, typename... Ts>
        void call_each_args(C &&c, Ts &&... args) {
            c->register_listener(this);
            call_each_args(std::forward<Ts>(args)...);
        }

        template<typename C>
        void call_each_args(C &&c) {
            c->register_listener(this);
        }

        void update() override {
            if (!Cell<VT>::has_value_) {
                auto value = bind_();
                if (value) {
                    Cell<VT>::set_value(std::move(value.value()));
                }
            }
        }

        using bind_type = decltype(std::bind(std::declval<std::function<boost::optional<VT>(T...)>>(),
                                             std::declval<T>()...));
        bind_type bind_;

    };

    template<typename F, typename... Args>
    auto make_cell(F&& f, Args&&... args) -> DerivedCell<typename decltype(f(args...))::value_type, Args...> {
        return DerivedCell<typename decltype(f(args...))::value_type, Args...>(std::forward<F>(f),
                                                                               std::forward<Args>(args)...);
    }

}

#endif //MSGRPC_CELL_H
