#ifndef MSGRPC_CELL_H
#define MSGRPC_CELL_H
#include <vector>
#include <boost/optional.hpp>

#include <iostream> //TODO:remove


//TODO: using std::move during cell value assignment.  by adding trace log in constructor to find out times of copy construction.

namespace msgrpc {

    struct Updatable {
        virtual void update() = 0;
    };

    struct CellBase {
        virtual ~CellBase() {}
    };

    template<typename T>
    struct Cell : CellBase {
        bool cell_has_value_{false};
        T value_;

        void set_value(T&& value) {
            std::cout << "cell got value:" << value << std::endl;
            value_ = std::move(value);
            cell_has_value_ = true;

            evaluate_all_derived_cells();
        }

        void evaluate_all_derived_cells() {
            for (int i = 0; i < updatables_.size(); ++i) {
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


}

#endif //MSGRPC_CELL_H
