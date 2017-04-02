#ifndef MSGRPC_CELL_H
#define MSGRPC_CELL_H
#include <list>
#include <boost/optional.hpp>

#include <iostream> //TODO:remove


//TODO: using std::move during cell value assignment.  by adding trace log in constructor to find out times of copy construction.

namespace msgrpc {

    struct Updatable {
        virtual ~Updatable() {}
        virtual void update() = 0;
    };


    template<typename T>
    struct Cell : Updatable {
        virtual ~Cell() {}
        bool cell_has_value_{false};
        T value_;

        void update() override {/**/}

        void set_value(T&& value) {
            //std::cout << "cell got value:" << value << std::endl;
            value_ = std::move(value);
            cell_has_value_ = true;

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

        std::vector<Updatable *> updatables_;
    };

    struct RpcContext {
        ~RpcContext() {
            std::cout << "~RpcContext" << std::endl;
            for (auto* r: release_list_) {
                std::cout << "context delete: " << r << std::endl;
                delete r;
            }
        }

        void track_item_to_release(Updatable* cell) {
            release_list_.push_back(cell);
        }

        std::list<Updatable*> release_list_;
    };
}

#endif //MSGRPC_CELL_H
