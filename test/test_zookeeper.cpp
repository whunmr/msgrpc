#include <iostream>
#include <gtest/gtest.h>
using namespace std;

#include <zookeeper/zookeeper.h>

static int connected = 0;
static int expired = 0;
void main_watcher(zhandle_t *zkh, int type, int state, const char *path, void *context) {
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            connected = 1;
        } else if (state == ZOO_CONNECTING_STATE) {
            connected = 0;
        } else if (state == ZOO_EXPIRED_SESSION_STATE) {
            expired = 1;
            connected = 0;
            zookeeper_close(zkh);
        }
    }
}

zhandle_t*  init(const char *hostPort) {
    srand(time(NULL));
    zoo_set_debug_level(ZOO_LOG_LEVEL_INFO);
    return zookeeper_init(hostPort, main_watcher, 15000, 0, 0, 0);
}

TEST(async_rpc, DISABLED_should_able_to_connect_to_zookeeper) {
    zhandle_t* zk = init("localhost:2181");
    EXPECT_TRUE(zk != nullptr);
    sleep(1);
    zookeeper_close(zk);
};

////////////////////////////////////////////////////////////////////////////////
#if 0

{
        return InterfaceYStub(ctxt)._____async_y(req);
}

{
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
                    ___bind_action(save_rsp_from_other_services_to_db, ___1);
                    ___bind_action(save_rsp_to_log, ___1);
        return ___1;
}


{
        auto ___1 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___2 = InterfaceYStub(ctxt)._____async_y(req);
        return ___bind_cell(merge_logic, ___1, ___2);
}

{
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
        {
            auto ___2 = ___bind_rpc(call__sync_y_again, ___1);
            {
                return ___bind_rpc(call__sync_y_again, ___2);
            }
        }
}

{
        auto ___1 = InterfaceYStub(ctxt).______sync_y_failed(req);
        {
            auto ___2 = ___bind_rpc(call__sync_y_again, ___1);
            {
                return ___bind_rpc(call__sync_y_again, ___2);
            }
        }
}


{
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
        {
            auto ___2 = ___bind_rpc(call__sync_y_failed, ___1);
            {
                return ___bind_rpc(call__sync_y_again, ___2);
            }
        }
}


{
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
        {
            auto ___2 = ___bind_rpc(call__sync_y_again, ___1);
            {
                return ___bind_rpc(call__sync_y_failed, ___2);
            }
        }
}


{
        auto ___3 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___1 = InterfaceYStub(ctxt)._____async_y(req); ___bind_action(action1, ___1);
        {
            auto ___2 = ___bind_cell(gen2, ___1);
            {
                return ___bind_cell(merge_logic, ___2, ___3);
            }
        }
}


{
        auto ___1 = InterfaceYStub(ctxt).______sync_y_failed(req);
        return ___bind_cell(gen5, ___1);
}


{
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req/*, ___ms(2000)*/);

        return ___1;
}


#endif