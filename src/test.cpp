
#include "tiny_sync_mq.h"
#include "tiny_async_mq.h"

#include <sys/time.h>
#include <unistd.h>

static void test_sync() {
    tiny_mq* tmq = tiny_sync_mq::getInstance(); 

    DataMsg<int> pdata(2, 100);
    uint64_t ch = 2;

    for (int i = 0; i < 10; ++i) {
        DataMsg<int> msg(ch, i * 6);
        tmq->put(ch, std::move(msg));
    }
    for (int i = 0; i < 10; ++i) {
        auto p = tmq->get(2);
        Msg* msg = p.get();
        DataMsg<int>* dm = dynamic_cast<DataMsg<int>*>(msg);
        if (msg) {
            printf("ch: %ld, msg payload : %d\n", ch, dm->getPayload());
        } else {
            printf("not found\n");
        }
    }
}

static void OnMessage(uint64_t chanId, Msg* userData) {
    DataMsg<int>* dm = dynamic_cast<DataMsg<int>*>(userData);
    printf("Asynchronize callback channel: %ld, msg payload : %d\n", chanId, dm->getPayload());
}

static void test_async() {
    tiny_mq* tmq = tiny_async_mq::getInstance();
    uint64_t ch = 9;
    tmq->subscribe(ch, OnMessage);
    tmq->start();

    std::thread th([&] {
        for (int i = 0; i < 10; ++i) {
            DataMsg<int> msg(ch, (i+1) * 7);
            tmq->put(ch, std::move(msg));
            printf("put msg %d\n", i);
            usleep(300000);
        }
    });
    th.join();
}

int main(int argc, char* argv[]) {
    //test_sync();
    test_async();

    return 0;
}
