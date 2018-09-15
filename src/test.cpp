
#include "tiny_sync_mq.h"
#include "tiny_async_mq.h"

static void test() {
    tiny_mq* tmq = tiny_sync_mq::getInstance();

    DataMsg<int> pdata(1);
    tmq->put(pdata);
}

int main(int argc, char* argv[]) {
    test();

    return 0;
}
