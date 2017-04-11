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

//TODO add service name in the rpc, and using service discovery
{
        return InterfaceYStub(ctxt)._____async_y(req);
}

{
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
                    ___bind_action(save_rsp_from_other_services_to_db, ___1);
                    ___bind_action(save_rsp_to_log, ___1);
        return ___1;
}

{///////////////////////////////////////////////////////////////////////////////TODO style 1
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
                    ___1 --> save_rsp_from_other_services_to_db;
                    ___1 --> save_rsp_to_log;
        return ___1;
}

{///////////////////////////////////////////////////////////////////////////////TODO other sytle 1:
        req = transform_req_func(req);

        auto ___1 = ___rpc(demo, InterfaceYStub, ______sync_y, req) {
                    ___step(save_rsp_from_other_services_to_db);
                    ___step(save_rsp_to_log);
        }

        return ___1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

{
        auto ___1 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___2 = InterfaceYStub(ctxt)._____async_y(req);
        return ___bind_cell(merge_logic, ___1, ___2);
}

{///////////////////////////////////////////////////////////////////////////////TODO
        auto ___1 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___2 = InterfaceYStub(ctxt)._____async_y(req);
        auto ___3 = (___1, ___2) --> merge_logic;
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

{///////////////////////////////////////////////////////////////////////////////TODO
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
        auto ___2 = ___1 --> call__sync_y_again;
        auto ___3 = ___2 --> call__sync_y_again;
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

{///////////////////////////////////////////////////////////////////////////////TODO
                auto ___3 =   InterfaceYStub(ctxt)._____async_y(req);
                auto ___1 =   InterfaceYStub(ctxt)._____async_y(req);
                     ___1 --> action1;
         auto ___2 = ___1 --> gen2;
         auto ___4 = (___2, ___3) --> merge_logic;
}

{///////////////////////////////////////////////////////////////////////////////TODO
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(2000), rollback_transaction);
        return ___1;
}

{///////////////////////////////////////////////////////////////////////////////TODO2:
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(2000), retry(3times), rollback_transaction);
        return ___1;
}

{///////////////////////////////////////////////////////////////////////////////TODO
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(2000), rollback_transaction);
        auto ___2 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(6000));
        auto ___3 = (___1, ___2) --> merge_logic;
        return ___3;
}


{///////////////////////////////////////////////////////////////////////////////TODO
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(2000), rollback_transaction);
        auto ___2 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(3000), rollback_transaction);
        auto ___3 = (___1, ___2) --> merge_logic;
        return ___3;
}


Cell<ResponseBar>& init_another_rpc_request(RpcContext &ctxt, Cell<ResponseBar> *___r) {
    RequestFoo req; req.reqa = ___r->value().rspa;
    return *(InterfaceYStub(ctxt).______sync_y_failed(req));
}

Cell<ResponseBar>& init_another_rpc_request(RpcContext &ctxt, Cell<ResponseBar> *___r) {
    RequestFoo req; req.reqa = ___r->value().rspa;
    return *(InterfaceYStub(ctxt).______sync_y_failed(req)) --> merge_logic --> timeout(___ms(5000), rollback_transaction);
}

{///////////////////////////////////////////////////////////////////////////////TODO
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(2000), rollback_transaction);
        auto ___2 = InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(3000), rollback_transaction);
        auto ___3 = (___1, ___2) --> init_another_rpc_request --> timeout(___ms(1000), rollback_transaction);
        return ___3;
}


{///////////////////////////////////////////////////////////////////////////////TODO
        auto ___1 = InterfaceYStub(ctxt).______sync_y(req);
             ___1 --> action1;

        auto ___2 = ___1 --> init_another_rpc_request;
             ___2 --> action1;
             ___2 --> action2;

        auto ___3 = ___2 --> init_another_rpc_request2;
             ___3 --> action3;

        auto ___4 = (___1, ___2) --> merge_logic --> timeout(___ms(5000), rollback_transaction);
        return ___4;
}


///////////////////////////////////////////////////////////////////////////////TODO
define_SI{
    auto ___1 = InterfaceYStub(ctxt).______sync_y(req);

    auto ___3 = ___1 --> InterfaceYStub(ctxt).______sync_y(req) --> timeout(___ms(200), rollback_transaction);
    auto ___4 = ___1 --> InterfaceYStub(ctxt).______sync_z(req) --> timeout(___ms(200), rollback_transaction);

    auto ___5 = (___3, ___4) --> another_rpc_request --> timeout(___ms(100));
    return ___5 ;

} ___as(si_foo, timeout___ms(250), retry(3), rollback_transaction);


//TODO multiple rpc, send rpc request to multiple interface provider.
//TODO add filter and map keywords, such as map, filter, all, any, success, failure

//TODO will not implement loop construct in SI, but we can call SI in loop in outside of SI.
//      but we need an loop style example
//
//      cell sending_following_ids(id_batch_index) {
//           if (id_batch_index == last_batch) { return loop_finish_cell(); };
//
//           auto rsp_cell =   SyncIdService.send_one_batch(get_ids_in_batch(id_batch_index));
//                rsp_cell --> sending_following_ids(rsp_cell.value.batch_id++);
//         return rsp_cell;
//      }
//
//      void on_sync_id_msg() {
//           sending_following_ids(start_batch_id);
//      }

#endif
