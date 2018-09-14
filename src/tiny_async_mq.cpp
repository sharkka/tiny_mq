/**
 * @File     tiny_async_mq.cpp
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */

#include "tiny_async_mq.h"


tiny_async_mq::~tiny_async_mq() {
    
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
    tiny_msq tm;
    tm.chan = chan;
    tm.userCallback = userCallback;
    return pushSubscriber(&tm, userCallback);
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
int tiny_async_mq::publish(uint64_t chan) {
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
    if (tmsg->chanId < 0) {
        printf("channel id invalid.\n");
        return -1;
    }
    auto chanQueue = msgPool_[tmsg->chanId];
    if (chanQueue == msgPool_.end()) {
        printf("channel not exist.\n");
        return -1;
    }
    chanQueue.userCallback = userCallback;
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
    if (tmsg->chanId < 0) {
        printf("channel id invalid.\n");
        return -1;
    }
    auto chanQueue = msgPool_[tmsg->chanId];
    if (chanQueue == msgPool_.end()) {
        printf("channel not exist.\n");
        return -1;
    }
    chanQueue.userCallback = nullptr;
    return 0;
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
    for (auto e : *pool) {
        std::thread th([this]{
            auto que = e->second;
            while (que.running_) {
                auto s = que.tq;
                if (s) {
                    que.userCallback(s->channelId(), s->get().get());
                }
            }
        });
        que.queueThread = std::move(th);
        que.queueThread.detach();
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
    for (auto e : *pool) {
        std::thread th([this]{
            auto que = e->second;
            que.running_ = false;
        });
    }
    return 0;    
}

