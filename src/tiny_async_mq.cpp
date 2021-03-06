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
 *  
 * @File     tiny_async_mq.cpp
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */
#include "tiny_async_mq.h"
#include <sys/time.h>
#include <unistd.h>
#include <memory.h>

tiny_mq* tiny_async_mq::getInstance() {
    if (nullptr == tmq_) {
        std::lock_guard<std::mutex> lock(instanceMtx_);
        if (nullptr == tmq_) {
            tmq_ = new tiny_async_mq;
        }
    }
    return tmq_;
}

tiny_async_mq::~tiny_async_mq() {
    destroy();
}
/**
 * @Method   destroy
 * @Brief
 * @DateTime 2018-09-17T14:45:07+0800
 * @Modify   2018-09-17T14:45:07+0800
 * @Author   Anyz
 */
void tiny_async_mq::destroy() {
    auto i = getInstance();
    i->stop();
    i->clean();
}
/**
 * @Method   clean
 * @Brief
 * @DateTime 2018-09-17T14:34:37+0800
 * @Modify   2018-09-17T14:34:37+0800
 * @Author   Anyz
 */
void tiny_async_mq::clean() {
    for (auto& e : msgPool_) {
        auto& que = e.second;
        this->popSubscriber(que.tq->channelId());
    }
    msgPool_.clear();
    printf("message queue pool has been clean up.\n");
}
/**
 * @Method   subscribe
 * @Brief
 * @DateTime 2018-09-17T14:34:34+0800
 * @Modify   2018-09-17T14:34:34+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @return   [description]
 */
int tiny_async_mq::subscribe(int chanId) {
    return pushSubscriber(chanId, nullptr);
}
/**
 * @Method   subscribe
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    userId [description]
 * @param    userCallback [description]
 * @return   [description]
 */
int tiny_async_mq::subscribe(int chanId, UserCallback userCallback) {
    return pushSubscriber(chanId, userCallback);
}
/**
 * @Method   unsubscribe
 * @Brief
 * @DateTime 2018-09-14T18:22:56+0800
 * @Modify   2018-09-14T18:22:56+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @return   [description]
 */
int tiny_async_mq::unsubscribe(int chanId) {
    return popSubscriber(chanId);
}
/**
 * @Method   registerEvent
 * @Brief
 * @DateTime 2018-09-14T18:22:45+0800
 * @Modify   2018-09-14T18:22:45+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    userCallback [description]
 * @return   [description]
 */
int tiny_async_mq::registerEvent(int chanId, UserCallback userCallback) {
    return 0;
}
/**
 * @Method   publish
 * @Brief
 * @DateTime 2018-09-14T18:22:38+0800
 * @Modify   2018-09-14T18:22:38+0800
 * @Author   Anyz
 * @param    complexQueue [description]
 * @return   [description]
 */
int tiny_async_mq::publish(tiny_complex_queue* complexQueue) {
    auto upmsg = complexQueue->tq->get();
    auto msg = upmsg.get();
    for (auto& cb : complexQueue->userCallbacks)
        cb(complexQueue->tq->channelId(), msg);
    return 0;
}
/**
 * @Method   put
 * @Brief
 * @DateTime 2018-09-17T11:37:30+0800
 * @Modify   2018-09-17T11:37:30+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    msg [description]
 * @return   [description]
 */
int tiny_async_mq::put(int chanId, TinyMsg&& msg) {
    printf("NOT IMPLEMENT\n");
    return 0;
}

/**
 * @Method   {{tag}}
 * @Brief
 * @DateTime 2018-09-19T11:41:28+0800
 * @Modify   2018-09-19T11:41:28+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    msg [description]
 * @return   [description]
 */
int tiny_async_mq::put(int chanId, tiny_message* msg) {
    auto e = msgPool_.find(chanId);
    if (e == msgPool_.end()) {
        printf("channel %d not found, message would not put into queue.\n", chanId);
        return -1;
    }
    tiny_complex_queue complexQueue;
    memset(&complexQueue, 0, sizeof(tiny_complex_queue));
    tiny_queue* tq = nullptr;
    std::mutex mtx;
    complexQueue.tq = tq;
    complexQueue.mtx = &mtx;
    if (e == msgPool_.end()) {
        tq = new tiny_queue;
        std::lock_guard<std::mutex> lck(*complexQueue.mtx);
        msg->setRef(1);
        tq->put(msg);
        msgPool_.insert(std::pair<int, tiny_complex_queue>(chanId, complexQueue));
        return 0;
    }
    std::lock_guard<std::mutex> lck(*complexQueue.mtx);
    msg->setRef(msgPool_[chanId].userCallbacks.size());
    msgPool_[chanId].tq->put(msg);
    return 0;
}
/**
 * @Method   pushSubscriber
 * @Brief
 * @DateTime 2018-09-14T18:22:26+0800
 * @Modify   2018-09-14T18:22:26+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    userCallback [description]
 * @return   [description]
 */
int tiny_async_mq::pushSubscriber(int chanId, UserCallback userCallback) {
    if (chanId < 0) {
        printf("channel id invalid.\n");
        return -1;
    }
    tiny_complex_queue complexQueue;
    memset(&complexQueue, 0, sizeof(tiny_complex_queue));
    auto chanQueue = msgPool_.find(chanId);
    if (chanQueue != msgPool_.end()) {   
    } else {
        printf("create new channel %d.\n", chanId);
        tiny_queue* tq = new tiny_queue;
        tq->setChannel(chanId);
        complexQueue.tq = tq;
        msgPool_.insert(std::pair<int, tiny_complex_queue>(chanId, complexQueue));
    }
    msgPool_[chanId].userCallbacks.push_back(userCallback);
    return 0;
}
/**
 * @Method   popSubscriber
 * @Brief
 * @DateTime 2018-09-14T18:22:07+0800
 * @Modify   2018-09-14T18:22:07+0800
 * @Author   Anyz
 * @param    tmsg [description]
 * @return   [description]
 */
int tiny_async_mq::popSubscriber(int chanId) {
    if (chanId < 0) {
        printf("channel id invalid.\n");
        return -1;
    }
    auto chanQueue = msgPool_.find(chanId);
    if (chanQueue != msgPool_.end()) {
        for (auto& v : msgPool_[chanId].userCallbacks)
            v = nullptr;
        if (msgPool_[chanId].tq) {
            delete msgPool_[chanId].tq;
            msgPool_[chanId].tq = nullptr;
        }
        return 0;
    } else {
        printf("channel not exist.\n");
        return -1;
    }
}
/**
 * @Method   start
 * @Brief
 * @DateTime 2018-09-14T18:36:16+0800
 * @Modify   2018-09-14T18:36:16+0800
 * @Author   Anyz
 * @return   [description]
 */
int tiny_async_mq::start() {
    auto pool = this->poolHandle();
    for (auto& e : *pool) {
        auto& que = e.second;
        que.running = true;
        std::thread* th = new std::thread([&, this]{
            while (que.running) {
                auto s = que.tq;
                if (s) {
                    publish(&que);
                    //usleep(1000000);
                } else {
                    usleep(200000);
                    printf("tiny queue is null\n");
                }
            }
        });
        th->detach();
        que.queueThread = std::move(th);
    }
    return 0;
}
/**
 * @Method   stop
 * @Brief
 * @DateTime 2018-09-14T18:36:18+0800
 * @Modify   2018-09-14T18:36:18+0800
 * @Author   Anyz
 * @return   [description]
 */
int tiny_async_mq::stop() {
    auto pool = this->poolHandle();
    for (auto& e : *pool) {
        auto& que = e.second;
        que.running = false;
        if (que.queueThread) {
            delete que.queueThread;
            que.queueThread = nullptr;
        }
    }
    printf("publish thread stoped.\n");
    return 0;    
}
