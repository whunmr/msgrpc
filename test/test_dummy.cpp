#include <iostream>
#include <gtest/gtest.h>
using namespace std;

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
        //TODO: invoke callback
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
    Cell<RspC> next_prime_number_async(const Req &req_value) {
//        RspC rsp_c;
//        rsp_c.rspc_value = 32;
//        auto rsp_ret = Cell<RspC>();
//        rsp_ret.value_ = rsp_c;
//        rsp_ret.has_value_ = true;
        //return Cell<RspC>();
        return result_cell_of_c;
    }
};

Cell<Rsp> BuzzMath::next_prime_number_async(const Req &req_value) {
    Cell<Rsp> result;
    ////B ----> C

    //interface_on_c.method_x(111);
    CccMath cccMath;
    result_cell_of_c = cccMath.next_prime_number_async(req_value);
    result_cell_of_c.bind(
        [](RspC& rsp_c){
            cout << "got result of rsp_c, value:" << rsp_c.rspc_value << endl;

            
        }
    );

    return result;
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

    Cell<Rsp> rpc_ret_async = buzz.next_prime_number_async(req);
    rpc_ret_async.bind(
        [](Rsp &) {
            cout << "in callback" << endl;
        }
    );

    RspC rsp_c {22};
    result_cell_of_c.set_value(rsp_c);
    //C---------->B (msg)
    //Rsp rsp; rsp.rsp_value = 31;
    //TODO: 1 find rpc_ret_async
    //rpc_ret_async.set_value(rsp);

    //TODO: timeout
}

//TODO: get/generate context_id from msg