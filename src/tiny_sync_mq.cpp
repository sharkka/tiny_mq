/**
 * @File     tiny_sync_mq.cpp
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */

#include "tiny_sync_mq.h"


tiny_sync_mq::~tiny_sync_mq() {
    // release and delete tiny_queue  
}

tiny_mq* tiny_sync_mq::getInstance() {
    if (nullptr == tmq_) {
        std::lock_guard<std::mutex> lock(instanceMtx_);
        if (nullptr == tmq_) {
            tmq_ = new tiny_sync_mq;
        }
    }
    return tmq_;
}
/**
 * @Method   put
 * @Brief
 * @DateTime 2018-09-14T18:32:11+0800
 * @Modify   2018-09-14T18:32:11+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    msg [description]
 * @return   [description]
 */
int tiny_sync_mq::put(uint64_t chanId, TinyMsg&& msg) {
    auto e = msgPool_.find(chanId);
    tiny_complex_queue complexQueue;
    tiny_queue* tq = new tiny_queue;
    std::mutex mtx;
    complexQueue.tq = tq;
    complexQueue.mtx = &mtx;
    if (e == msgPool_.end()) {
        tq->put(std::move(msg));
        msgPool_.insert(std::pair<uint64_t, tiny_complex_queue>(chanId, complexQueue));
        return 0;
    }
    std::lock_guard<std::mutex> lck(*complexQueue.mtx);
    msgPool_[chanId].tq->put(std::move(msg));
    return 0;
}
/**
 * @Method   get
 * @Brief
 * @DateTime 2018-09-14T18:32:11+0800
 * @Modify   2018-09-14T18:32:11+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    msg [description]
 * @return   [description]
 */
std::unique_ptr<Msg> tiny_sync_mq::get(uint64_t chanId, int millisec) {
    auto e = msgPool_.find(chanId);
    if (e == msgPool_.end()) {
        printf("channel %ld not found\n", chanId);
        return nullptr;
    }
    return msgPool_[chanId].tq->get(millisec);
}

