
#include "tiny_sync_mq.h"
#include "tiny_async_mq.h"

static void test() {
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
            printf("ch: %d, msg payload : %d\n", ch, dm->getPayload());
        } else {
            printf("not found\n");
        }
    }
}

int main(int argc, char* argv[]) {
    test();

    return 0;
}
