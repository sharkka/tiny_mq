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
 * @File     tiny_sync_mq.cpp
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */

#include "tiny_sync_mq.h"
#include <memory.h>

tiny_sync_mq::~tiny_sync_mq() {
    // release and delete tiny_queue  
}
/**
 * @Method   getInstance
 * @Brief
 * @DateTime 2018-09-14T18:32:11+0800
 * @Modify   2018-09-14T18:32:11+0800
 * @Author   Anyz
 * @param    chanId [description]
 * @param    msg [description]
 * @return   [description]
 */
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
int tiny_sync_mq::put(int chanId, TinyMsg&& msg) {
    auto e = msgPool_.find(chanId);
    tiny_complex_queue complexQueue;
    memset(&complexQueue, 0, sizeof(tiny_complex_queue));
    tiny_queue* tq = new tiny_queue;
    std::mutex mtx;
    complexQueue.tq = tq;
    complexQueue.mtx = &mtx;
    if (e == msgPool_.end()) {
        tq->put(std::move(msg));
        msgPool_.insert(std::pair<int, tiny_complex_queue>(chanId, complexQueue));
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
std::shared_ptr<tiny_message> tiny_sync_mq::get(int chanId, int millisec) {
    auto e = msgPool_.find(chanId);
    if (e == msgPool_.end()) {
        printf("channel %d not found\n", chanId);
        return nullptr;
    }
    return msgPool_[chanId].tq->get(millisec);
}

