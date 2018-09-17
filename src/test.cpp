
#include "tiny_sync_mq.h"
#include "tiny_async_mq.h"

#include <sys/time.h>
#include <unistd.h>

/**
 * @Method   test_sync
 * @Brief    put & get mode
 * @DateTime 2018-09-17T14:06:49+0800
 * @Modify   2018-09-17T14:06:49+0800
 * @Author   Anyz
 */
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
/**
 * @Method   OnMessage11
 * @Brief    message arrived callback
 * @DateTime 2018-09-17T14:07:13+0800
 * @Modify   2018-09-17T14:07:13+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    userData [description]
 */
static void OnMessage1(uint64_t chanId, Msg* userData) {
    DataMsg<int>* dm = dynamic_cast<DataMsg<int>*>(userData);
    printf("1 Asynchronize callback channel: %ld, msg payload : %d\n", chanId, dm->getPayload());
}
/**
 * @Method   OnMessage2
 * @Brief    message arrived callback
 * @DateTime 2018-09-17T14:07:25+0800
 * @Modify   2018-09-17T14:07:25+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    userData [description]
 */
static void OnMessage2(uint64_t chanId, Msg* userData) {
    DataMsg<int>* dm = dynamic_cast<DataMsg<int>*>(userData);
    printf("2 Asynchronize callback channel: %ld, msg payload : %d\n", chanId, dm->getPayload());
}
/**
 * @Method   test_async
 * @Brief    subscribe & publish mode, one user subscribe special channel and message will
 *           be publish at the channel on meessage arrived.
 * @DateTime 2018-09-17T14:07:59+0800
 * @Modify   2018-09-17T14:07:59+0800
 * @Author   Anyz
 */
static void test_async() {
    AutoRelease();
    tiny_mq* tmq = tiny_async_mq::getInstance();
    tiny_mq* tmqCheck = tiny_async_mq::getInstance();
    printf("%p == %p\n", tmq, tmqCheck);

    uint64_t ch = 9;
    tmq->subscribe(ch, OnMessage1);
    tmq->subscribe(ch+1, OnMessage2);
    tmq->start();

    std::thread th1([&] {
        for (int i = 0; i < 100; ++i) {
            DataMsg<int> msg(ch, (i+1) * 7);
            tmq->put(ch, std::move(msg));
            printf("1 put msg payload : %d\n", i);
            usleep(10000);
        }
    });

    std::thread th2([&] {
        for (int i = 0; i < 100; ++i) {
            DataMsg<int> msg(ch+1, (i+1) * 2);
            tmq->put(ch+1, std::move(msg));
            printf("2 put msg payload : %d\n", i);
            usleep(20000);
        }
    });

    th1.join();
    th2.join();
}
/**
 * @Method   main
 * @Brief    test
 * @DateTime 2018-09-17T14:10:38+0800
 * @Modify   2018-09-17T14:10:38+0800
 * @Author   Anyz
 * @param    argc [description]
 * @param    argv [description]
 * @return   [description]
 */
int main(int argc, char* argv[]) {
    //test_sync();
    test_async();

    return 0;
}
