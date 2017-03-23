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


