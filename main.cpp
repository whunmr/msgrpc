#include <iostream>

#include "gen1/api_types.h"

using namespace org::example::msgrpc;

int main() {
    std::cout << "xx" << std::endl;

    thrift::ResponseData rsp;
    rsp.pet_id = 11;
    rsp.pet_name = "pet_name_foo";
    rsp.pet_weight = 23;



    return 0;
}

