#ifndef MSGRPC_MSGRPC_TEST_H
#define MSGRPC_MSGRPC_TEST_H

#include <condition_variable>
#include <gtest/gtest.h>
#include <thread>
#include <adapter_example/details/msgrpc_test_loop.h>

bool can_safely_exit = false;
std::mutex can_safely_exit_mutex;
std::condition_variable can_safely_exit_cv;

struct MsgRpcTest : public ::testing::Test {
    virtual void SetUp() {
        can_safely_exit = false;

        msgrpc::RpcSequenceId::instance().reset();
        demo::TimerMgr::instance().reset();

        demo::ZkServiceRegister::instance().register_service("demo_service_x", "127.0.0.1:6666");
        demo::ZkServiceRegister::instance().register_service("demo_service_y", "127.0.0.1:7777");
    }

    virtual void TearDown() {
        std::unique_lock<std::mutex> lk(can_safely_exit_mutex);
        can_safely_exit_cv.wait(lk, []{return can_safely_exit;});
    }
};

struct test_thread : std::thread {
    template<typename... Args>
    test_thread(Args... args) : std::thread(msgrpc_test_loop, args...) { /**/ }

    ~test_thread() {
        join();
        //valgrind bug: https://bugs.kde.org/show_bug.cgi?id=349128
    }
};

void create_delayed_exiting_thread() {
    std::thread thread_delayed_exiting(
            []{
                std::this_thread::sleep_for(std::chrono::milliseconds(200));

                std::lock_guard<std::mutex> lk(can_safely_exit_mutex);
                can_safely_exit = true;
                can_safely_exit_cv.notify_one();

                UdpChannel::close_all_channels();
            });
    thread_delayed_exiting.detach();
}


#endif //MSGRPC_MSGRPC_TEST_H
