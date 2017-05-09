#include <test_util/test_runner.h>
#include <cstddef>
#include <iostream>
#include <vector>

namespace {
    std::vector<TestCaseBase*>& get_testcases() {
        static std::vector<TestCaseBase*> g_testcases;
        return g_testcases;
    }

    void do_run_testcase(size_t index) {
        if (index < get_testcases().size()) {
            std::cout << "\n-------------------" << std::endl << "[RUN] testcase: " << index << std::endl;
            get_testcases()[index]->run();
        } else {
            std::cout << "------------------------" << std::endl << "[ALL testcases finished]" << std::endl;
            exit(0);
        }
    }
}

TestCaseBase::TestCaseBase() {
    get_testcases().push_back(this);
}

void run_next_testcase() {
    static size_t index = 0;
    do_run_testcase(index++);
}
