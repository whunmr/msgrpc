#ifndef MSGRPC_CELL_H
#define MSGRPC_CELL_H
#include <list>
#include <boost/optional.hpp>

#include <iostream> //TODO:remove


//TODO: using std::move during cell value assignment.  by adding trace log in constructor to find out times of copy construction.

namespace msgrpc {

    enum class RpcResult : unsigned short {
        succeeded = 0
        , deferred = 1
        , failed  = 2
        , timeout = 3
        , iface_not_found =  4
        , method_not_found = 5
    };

    struct Updatable {
        virtual ~Updatable() {}
        virtual void update() = 0;
    };

    template<typename T>
    struct CellBase : Updatable {
        typedef T value_type;

        virtual ~CellBase() { }

        bool has_value_{false};
        RpcResult status_ = {RpcResult::succeeded};

        void update() override {/**/}

        void set_failed_reason(RpcResult ret) {
            status_ = ret;
            evaluate_all_derived_cells();
        }

        RpcResult failed_reason() const {
            return status_;
        }

        RpcResult status() const {
            return status_;
        }

        bool is_failed() const {
            return status_ != RpcResult::succeeded;
        }

        void set_cell_value(const CellBase<T>& rhs) {
            if (rhs.is_failed()) {
                this->set_failed_reason(rhs.failed_reason());
            } else {
                this->set_value(rhs.value());
            }
        }

        void set_value(const T& value) {
            value_ = value;
            has_value_ = true;
            evaluate_all_derived_cells();
        }

        void set_value(T&& value) {
            value_ = std::move(value);
            has_value_ = true;
            evaluate_all_derived_cells();
        }

        void evaluate_all_derived_cells() {
            size_t size = updatables_.size();

            for (int i = 0; i < size; ++i) {
                if (updatables_[i] != nullptr) {
                    updatables_[i]->update();
                }
            }
        }

        void register_listener(Updatable *updatable) {
            updatables_.push_back(updatable);
        }

        operator T&() {
            return value_;
        }

        const T& value() const {
            assert(this->has_value_ && "can not access value_ field, if cell do not has_value_.");
            return value_;
        }

    protected:
        std::vector<Updatable *> updatables_;

    private:
        T value_;
    };

    struct RpcContext {
        ~RpcContext() {
            for (auto* r: release_list_) {
                delete r;
            }
        }

        template<typename T>
        T track(T cell) {
            release_list_.push_back(static_cast<Updatable*>(cell));
            return cell;
        }

        std::list<Updatable*> release_list_;
    };
}

#endif //MSGRPC_CELL_H
