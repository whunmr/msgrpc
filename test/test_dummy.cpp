#include <gtest/gtest.h>

#include "msgrpc/frp/cell.h"

using namespace std;
using namespace msgrpc;

boost::optional<int> derive_logic_from_a_to_b(Cell<int> *a) {
    if (a->has_value_) {
        cout << " a ---> b" << endl;
        return boost::make_optional(a->value_ * 3);
    }
    return {};
}

boost::optional<int> derive_logic_from_b_to_c(Cell<int>* b) {
    if (b->has_value_) {
        cout << " b ---> c" << endl;
        return boost::make_optional(b->value_ + 1);
    }
    return {};
}

boost::optional<int> derive_logic_from_a_and_f_to_e(Cell<int> *a, Cell<int> *f) {
    if ( a->has_value_ && f->has_value_) {
        cout << " a&c ---> e:" << " derive value of e from a and c, a:" << a->value_ << " c:" << f->value_ << endl;
        return boost::make_optional(a->value_ + f->value_);
    }
    return {};
}

TEST(async_rpc, test_______________000) {
    Cell<int> a;
    Cell<int> f;

    auto b = make_cell(derive_logic_from_a_to_b, &a);
    auto c = make_cell(derive_logic_from_b_to_c, &b);
    auto e = make_cell(derive_logic_from_a_and_f_to_e, &a, &f);
    auto g = make_cell(derive_logic_from_a_and_f_to_e, &a, &f);

    a.set_value(33);
    f.set_value(11);
};

////////////////////////////////////////////////////////////////////////////////
void init_test();

TEST(async_rpc, test_dummy_future) {
    init_test();
};

struct Req {
    int req_value;
};

struct Rsp {
    int rsp_value;
    friend ostream& operator<<(ostream& os, const Rsp& dt);
};

ostream& operator<<(ostream& os, const Rsp& rsp)
{
    os << rsp.rsp_value;
    return os;
}

struct RspC {
    int rspc_value;
    friend ostream& operator<<(ostream& os, const RspC& dt);
};

ostream& operator<<(ostream& os, const RspC& rsp)
{
    os << rsp.rspc_value;
    return os;
}

struct BuzzMath {
    Cell<Rsp> next_prime_number_sync(const Req &req_value);
    Cell<Rsp>& next_prime_number_async(const Req &req_value);
};

Cell<RspC> result_cell_of_c;

struct CccMath {
    Cell<RspC> c_next_prime_value(const Req &req_value) {
        return result_cell_of_c;
    }
};

Cell<Rsp> result_of_b;

boost::optional<Rsp> derive_b_from_c_result(Cell<RspC>* rsp_c) {
    if (rsp_c->has_value_) {
        Rsp rsp_b {rsp_c->value_.rspc_value * 5};
        return boost::make_optional(rsp_b);
    }
    return {};
}

Cell<Rsp>& BuzzMath::next_prime_number_async(const Req &req_value) {
    CccMath cccMath;
    result_cell_of_c = cccMath.c_next_prime_value(req_value);

    static auto e = make_cell(derive_b_from_c_result, &result_cell_of_c);
    return e;
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
    if (rpc_ret.has_value_) {
        cout << "B buzz.next_prime_number_sync: send result msg to sender." << endl;
    }

    result_of_b = buzz.next_prime_number_async(req);

    RspC rsp_c {11};
    result_cell_of_c.set_value(std::move(rsp_c));
}

//TODO: get/generate context_id from msg