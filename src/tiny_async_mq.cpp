/**
 * @File     tiny_async_mq.cpp
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */

#include "tiny_async_mq.h"
#include <sys/time.h>
#include <unistd.h>

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
 * @param    chan [description]
 * @return   [description]
 */
int tiny_async_mq::subscribe(uint64_t chan) {
    return pushSubscriber(chan, nullptr);
}
/**
 * @Method   subscribe
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 * @param    chan [description]
 * @param    userId [description]
 * @param    userCallback [description]
 * @return   [description]
 */
int tiny_async_mq::subscribe(uint64_t chan, UserCallback userCallback) {
    return pushSubscriber(chan, userCallback);
}
/**
 * @Method   unsubscribe
 * @Brief
 * @DateTime 2018-09-14T18:22:56+0800
 * @Modify   2018-09-14T18:22:56+0800
 * @Author   Anyz
 * @param    chan [description]
 * @return   [description]
 */
int tiny_async_mq::unsubscribe(uint64_t chan) {
    return popSubscriber(chan);
}
/**
 * @Method   registerEvent
 * @Brief
 * @DateTime 2018-09-14T18:22:45+0800
 * @Modify   2018-09-14T18:22:45+0800
 * @Author   Anyz
 * @param    chan [description]
 * @param    userCallback [description]
 * @return   [description]
 */
int tiny_async_mq::registerEvent(uint64_t chan, UserCallback userCallback) {
    return 0;
}
/**
 * @Method   publish
 * @Brief
 * @DateTime 2018-09-14T18:22:38+0800
 * @Modify   2018-09-14T18:22:38+0800
 * @Author   Anyz
 * @param    chan [description]
 * @return   [description]
 */
int tiny_async_mq::publish(tiny_complex_queue* complexQueue) {
    auto upmsg = complexQueue->tq->get();
    auto msg = upmsg.get();
    complexQueue->userCallback(complexQueue->tq->channelId(), msg);
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
int tiny_async_mq::put(uint64_t chanId, TinyMsg&& msg) {
    auto e = msgPool_.find(chanId);
    tiny_complex_queue complexQueue;
    tiny_queue* tq = nullptr;
    std::mutex mtx;
    complexQueue.tq = tq;
    complexQueue.mtx = &mtx;
    if (e == msgPool_.end()) {
        tq = new tiny_queue;
        tq->put(std::move(msg));
        msgPool_.insert(std::pair<uint64_t, tiny_complex_queue>(chanId, complexQueue));
        return 0;
    }
    std::lock_guard<std::mutex> lck(*complexQueue.mtx);
    msgPool_[chanId].tq->put(std::move(msg));
    return 0;
}
/**
 * @Method   pushSubscriber
 * @Brief
 * @DateTime 2018-09-14T18:22:26+0800
 * @Modify   2018-09-14T18:22:26+0800
 * @Author   Anyz
 * @param    chan [description]
 * @param    userCallback [description]
 * @return   [description]
 */
int tiny_async_mq::pushSubscriber(uint64_t chan, UserCallback userCallback) {
    if (chan < 0) {
        printf("channel id invalid.\n");
        return -1;
    }
    tiny_complex_queue complexQueue;
    auto chanQueue = msgPool_.find(chan);
    if (chanQueue != msgPool_.end()) {   
    } else {
        printf("create new channel %ld.\n", chan);
        tiny_queue* tq = new tiny_queue;
        tq->setChannel(chan);
        complexQueue.tq = tq;
        msgPool_.insert(std::pair<uint64_t, tiny_complex_queue>(chan, complexQueue));
    }
    msgPool_[chan].userCallback = userCallback;
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
int tiny_async_mq::popSubscriber(uint64_t chan) {
    if (chan < 0) {
        printf("channel id invalid.\n");
        return -1;
    }
    auto chanQueue = msgPool_.find(chan);
    if (chanQueue != msgPool_.end()) {
        msgPool_[chan].userCallback = nullptr;
        if (msgPool_[chan].tq) {
            delete msgPool_[chan].tq;
            msgPool_[chan].tq = nullptr;
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
                } else {
                    usleep(200000);
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

