#include <iostream>
#include <gtest/gtest.h>
#include <list>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// a <--- func <---- b

struct Updatable {
    virtual void update() = 0;
};


template<typename T>
struct CellX {
    bool has_value_ {false};
    T value_;

    void set_value(T&& value) {
        cout << "binded to value:" << value << endl;

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

    void register_listener(Updatable* updatable) {
        updatables_.push_back(updatable);
    }

    std::list<Updatable*> updatables_;
};

template<typename VT, typename... T>
struct DerivedCell : CellX<VT>, Updatable {
    using bind_type = decltype(std::bind(std::declval<std::function<VT(T...)>>(),std::declval<T>()...));

    DerivedCell(std::function<VT(T...)> logic, T&&... args)
        : bind_(logic, std::forward<T>(args)...) {

        call_each_args(std::forward<T>(args)...);
    }

    template<typename C, typename... Ts>
    void call_each_args(C&& c, Ts&&... args) {
        c->register_listener(this);
        call_each_args(std::forward<Ts>(args)...);
    }

    template<typename C>
    void call_each_args(C&& c) {
        c->register_listener(this);
    }

    bind_type bind_;

    void update() override {
        CellX<VT>::set_value(bind_());
    }
};

template <typename VT, typename F, typename... Args>
DerivedCell<VT, Args...> make_derived_cell(F&& f, Args&&... args)
{
    return DerivedCell<VT, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
}

int derive_logic_from_b_to_a(CellX<int>* a) {
    return a->value_ * 3;
}

TEST(async_rpc, test_______________000) {
    CellX<int> a;

    auto b = make_derived_cell<int>(derive_logic_from_b_to_a, &a);

    a.set_value(33);
};

////////////////////////////////////////////////////////////////////////////////
// rc <---- rpc_result
// rd <---- rpc_result
// a <---- (b <--- rc) && (rd)
TEST(async_rpc, test_______________001) {

};

////////////////////////////////////////////////////////////////////////////////
void init_test();

TEST(async_rpc, test_dummy_future) {
    init_test();
};

#if 0
//A  -->  B  -->  C
          B  -->  D
  A  <--  B

//X  -->  A  --> B
#endif

struct Req {
    int req_value;
};

struct Rsp {
    int rsp_value;
};

struct RspC {
    int rspc_value;
};

template<typename T>
struct Cell {
    typedef std::function<void(T&)> callback_type;

    bool has_value_ {false};
    T value_;

    void set_value(T& t) {
        if (callback_)
            callback_(t);
    }

    void bind(callback_type callback) {
        if (this->has_value_) {
            callback(value_);
        } else {
            callback_ = callback;
        }
    }

    callback_type callback_;
};

struct BuzzMath {
    Cell<Rsp> next_prime_number_sync(const Req &req_value);
    Cell<Rsp> next_prime_number_async(const Req &req_value);
};

Cell<RspC> result_cell_of_c;

struct CccMath {
    Cell<RspC> c_next_prime_value(const Req &req_value) {
        return result_cell_of_c;
    }
};

Cell<Rsp> result_of_b;



Cell<Rsp> BuzzMath::next_prime_number_async(const Req &req_value) {
    CccMath cccMath;
    result_cell_of_c = cccMath.c_next_prime_value(req_value);
    result_cell_of_c.bind(
        [](RspC& rsp_c){
            cout << "got result of rsp_c, value:" << rsp_c.rspc_value << endl;
            Rsp rsp_b {rsp_c.rspc_value * 2};
            result_of_b.set_value(rsp_b);
        }
    );

    return result_of_b;
}

Cell<Rsp> BuzzMath::next_prime_number_sync(const Req &req_value) {
    Rsp rsp; rsp.rsp_value = 29;

    Cell<Rsp> rsp_ret = Cell<Rsp>();
    rsp_ret.value_ = rsp;
    rsp_ret.has_value_ = true;

    return rsp_ret;
}

void init_test() {
    BuzzMath buzz;
    Req req = {23};

    Cell<Rsp> rpc_ret = buzz.next_prime_number_sync(req);
    rpc_ret.bind(
        [](Rsp &) {
            cout << "B buzz.next_prime_number_sync: send result msg to sender." << endl;
        }
    );

    result_of_b = buzz.next_prime_number_async(req);
    result_of_b.bind(
        [](Rsp& rsp) {
            cout << "in callback of B: got value:" << rsp.rsp_value << endl;
        }
    );

    RspC rsp_c {22};
    result_cell_of_c.set_value(rsp_c);
}

//TODO: get/generate context_id from msg

