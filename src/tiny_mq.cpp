/**
 * @File     tiny_mq.cpp
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */

#include <sys/time.h>
#include "tiny_mq.h"
#include <memory.h>
#include <atomic>

std::mutex tiny_mq::instanceMtx_;
tiny_mq*   tiny_mq::tmq_        = nullptr;

/**
 * @Method   generateChannelId
 * @Brief
 * @DateTime 2018-09-14T18:33:04+0800
 * @Modify   2018-09-14T18:33:04+0800
 * @Author   Anyz
 * @return   [description]
 */
static int generateChannelId() {
    static std::atomic<int> id(1);
    return ++id;
}

tiny_mq::~tiny_mq() {
}
/**
 * @Method   setMaxChannels
 * @Brief
 * @DateTime 2018-09-14T16:04:57+0800
 * @Modify   2018-09-14T16:04:57+0800
 * @Author   Anyz
 * @param    maxCh [description]
 */
void tiny_mq::setMaxChannels(int maxCh) {
    maxChannels_ = maxCh;
}
/**
 * @Method   createChannel
 * @Brief
 * @DateTime 2018-09-14T16:08:31+0800
 * @Modify   2018-09-14T16:08:31+0800
 * @Author   Anyz
 * @param    chan [description]
 * @param    size [description]
 */
int tiny_mq::createChannel(int size) {
    tiny_complex_queue complexQueue;
    memset(&complexQueue, 0, sizeof(tiny_complex_queue));
    tiny_queue tq;
    complexQueue.tq = &tq;
    std::lock_guard<std::mutex> lck(poolMtx_);
    complexQueue.tq->setChannel(generateChannelId());
    complexQueue.tq->setMaxSize(size);
    int ch = generateChannelId();
    msgPool_.insert(std::pair<int, tiny_complex_queue>(ch, complexQueue));
    return ch;
}
/**
 * @Method   destroyChannel
 * @Brief
 * @DateTime 2018-09-14T16:08:31+0800
 * @Modify   2018-09-14T16:08:31+0800
 * @Author   Anyz
 * @param    chan [description]
 * @param    size [description]
 */
void tiny_mq::destroyChannel(int chanId) {
    std::lock_guard<std::mutex> lck(poolMtx_);
    auto e = msgPool_.find(chanId);
    if (e != msgPool_.end()) {
        msgPool_.erase(e);
    }
}


