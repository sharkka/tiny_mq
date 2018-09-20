/**
 * Copyright (C) <2018> <copyright Horizon Robotics>
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction,  
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,  
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is  
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "tiny_sync_mq.h"
#include "tiny_async_mq.h"

#include <sys/time.h>
#include <unistd.h>
#include <string>

typedef struct _user_msg_t {
    int id;
    double r;
    std::string name;
} user_msg_t;

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
    int ch = 2;

    for (int i = 0; i < 10; ++i) {
        DataMsg<int> msg(ch, i * 6);
        tmq->put(ch, std::move(msg));
    }
    for (int i = 0; i < 10; ++i) {
        auto p = tmq->get(2);
        tiny_message* msg = p.get();
        DataMsg<int>* dm = dynamic_cast<DataMsg<int>*>(msg);
        if (msg) {
            printf("ch: %d, msg payload : %d\n", ch, dm->getPayload());
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
static void OnMessage1(int chanId, tiny_message* userData) {
    DataMsg<int>* dm = dynamic_cast<DataMsg<int>*>(userData);
    printf("1 Asynchronize callback channel: %d, msg id: %lld, payload : %d\n",
        chanId,
        dm->getUniqueId(),
        dm->getPayload());
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
static void OnMessage2(int chanId, tiny_message* userData) {
    DataMsg<int>* dm = dynamic_cast<DataMsg<int>*>(userData);
    printf("2 Asynchronize callback channel: %d, msg id: %lld, payload : %d\n",
        chanId,
        dm->getUniqueId(),
        dm->getPayload());
}
/**
 * @Method   OnMessage3
 * @Brief
 * @DateTime 2018-09-19T15:19:08+0800
 * @Modify   2018-09-19T15:19:08+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    userData [description]
 */
static void OnMessage3(int chanId, tiny_message* userData) {
    DataMsg<user_msg_t>* dm = dynamic_cast<DataMsg<user_msg_t>*>(userData);
    printf("3 Asynchronize callback channel: %d, msg id: %lld, payload :(%d, %2.2f, %s)\n",
        chanId,
        dm->getUniqueId(),
        dm->getPayload().id, dm->getPayload().r, dm->getPayload().name.c_str());
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
    // step 1 declaration of auto memory garbage collector
    AutoCollector();
    // step 2 get instance
    tiny_mq* tmq = tiny_async_mq::getInstance();
    tiny_mq* tmqCheck = tiny_async_mq::getInstance();
    printf("%p == %p\n", tmq, tmqCheck);

    // step 3 subscribe one special channel
    int ch1 = 9;
    int ch2 = 10;
    tmq->subscribe(ch1, OnMessage1);
    tmq->subscribe(ch1, OnMessage2);
    tmq->subscribe(ch2, OnMessage3);
    // step 4
    tmq->start();
    // step 5 any user put, and subscriber will get message from callback
    //DataMsg<int>* msg;
    int N = 100000;
    std::thread th1([&] {
        for (int i = 0; i < N; ++i) {
            //DataMsg<int>* msg = new DataMsg<int>(ch1, (i+1)*7);
            DataMsg<int>* msg = DataMsg<int>::newDataMsg(ch1, (i+1) * 7);
            tmq->put(ch1, msg);
            printf("1 put msg payload : %d\n", i);
            usleep(10000);
        }
    });

    std::thread th2([&] {
        for (int i = 0; i < N; ++i) {
            user_msg_t umsg;
            umsg.id = 25;
            umsg.name = "Darge";
            umsg.r = 5.29 * i;
            DataMsg<user_msg_t>* msg = DataMsg<user_msg_t>::newDataMsg(ch2, umsg);
            //DataMsg<int> msg(ch2, (i+1)*2);
            tmq->put(ch2, msg);
            printf("2 put msg payload : %d\n", i);
            usleep(20000);
        }
    });

    th2.join();
    th1.join();
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
