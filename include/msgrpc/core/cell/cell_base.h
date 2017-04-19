#ifndef MSGRPC_CELL_BASE_H
#define MSGRPC_CELL_BASE_H
#include <list>
#include <boost/optional.hpp>
#include <msgrpc/core/rpc_result.h>

namespace msgrpc {

    struct Updatable {
        virtual ~Updatable() {}
        virtual void update() = 0;
    };

    struct CellStatus {
        virtual ~CellStatus() = default;

        void set_failed_reason(RpcResult ret) {

            status_ = ret;
            evaluate_all_derived_cells();
        }

        RpcResult failed_reason() const {
            return status_;
        }

        inline bool is_timeout() const {
            return status_ == RpcResult::timeout;
        }

        inline bool has_error() const {
            return status_ != RpcResult::succeeded;
        }

        inline bool has_value_or_error() const {
            return has_value() || has_error();
        }

        inline bool has_value() const {
            return has_value_;
        }

        inline bool is_empty() const {
            return ! has_value_or_error();
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
            assert(updatable != nullptr && "should not register null updateable object.");

            if (this->has_value_or_error()) {
                updatable->update();
            } else {
                updatables_.push_back(updatable);
            }
        }

      protected:
        std::vector<Updatable *> updatables_;

        bool has_value_{false};

        //cell's status:
        //  empty:   succeeded, not has_value_;
        //  value:   succeeded, has_value_;
        //  error:   failed or timeout
        RpcResult status_ = {RpcResult::succeeded};

        template<typename T> friend struct DefaultCell;
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

    struct CellContextBase {
        virtual ~CellContextBase() {
            if (context_ != nullptr) {
                delete context_;
                context_ = nullptr;
            }
        }

        void set_binded_context(RpcContext& context) {
            context_ = &context;
        }

        RpcContext* context_ = {nullptr};
    };

    template<typename T>
    struct CellBase : Updatable, CellStatus, CellContextBase {
        typedef T value_type;

        CellBase() : value_() { }
        virtual ~CellBase() { }

        void update() override {/**/}

        void set_cell_value(const CellBase<T>& rhs) {
            if (rhs.has_error()) {
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

        operator T&() {
            assert(has_value_);
            return value_;
        }

        const T& value() const {
            assert(this->has_value_ && "can not access value_ field, if cell do not has_value_.");
            return value_;
        }

    private:
        T value_;
    };
}

#endif //MSGRPC_CELL_BASE_H
