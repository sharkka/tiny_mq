/**
 * @File     tiny_mq.cpp
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */

#include <sys/time.h>
#include "tiny_mq.h"

/**
 * @Method   getInstance
 * @Brief
 * @DateTime 2018-09-14T14:26:17+0800
 * @Modify   2018-09-14T14:26:17+0800
 * @Author   Anyz
 * @return   [description]
 */
tiny_mq* tiny_mq::getInstance() {
    if (tmq_ == nullptr) {
        std::lock_guard<std::mutex> lock(instanceMtx_);
        if (tmq_ == nullptr) {
            tmq_ = new tiny_mq;
        }
    }
    return tmq_;
}

tiny_mq::~tiny_mq() {
    if (tmq_) {
        delete tmq_;
        tmq_ = nullptr;
    }
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
uint64_t tiny_mq::createChannel(int size) {
    tiny_async_queue taq;
    tiny_queue tq;
    taq.tq = &tq;
    std::lock_guard<std::mutex> lck(poolMtx_);
    taq.tq->setChannel(msgPool_.size() + 1);
    taq.tq->setMaxSize(size);
    uint64_t ch = generateChannelId();
    msgPool_.insert(std::pair<uint64_t, tiny_async_queue>(ch, taq));
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
void tiny_mq::destroyChannel(uint64_t chanId) {
    std::lock_guard<std::mutex> lck(poolMtx_);
    auto e = msgPool_.find(chanId);
    if (e != msgPool_.end()) {
        msgPool_.erase(e);
    }
}
/**
 * @Method   debugOn
 * @Brief
 * @DateTime 2018-09-14T16:08:45+0800
 * @Modify   2018-09-14T16:08:45+0800
 * @Author   Anyz
 */
void tiny_mq::debugOn(bool b) {
    debugFlag_ = b;
}
/**
 * @Method   dumpMqStatus
 * @Brief
 * @DateTime 2018-09-14T18:33:02+0800
 * @Modify   2018-09-14T18:33:02+0800
 * @Author   Anyz
 */
void tiny_mq::dumpMqStatus() {
    
}
/**
 * @Method   generateChannelId
 * @Brief
 * @DateTime 2018-09-14T18:33:04+0800
 * @Modify   2018-09-14T18:33:04+0800
 * @Author   Anyz
 * @return   [description]
 */
int tiny_mq::generateChannelId() {
    struct timeval start;
    gettimeofday(&start, nullptr);
    uint64_t value = start.tv_sec<<32 | start.tv_usec;
    return (uint64_t)value;
}

