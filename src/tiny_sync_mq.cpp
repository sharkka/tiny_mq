/**
 * @File     tiny_sync_mq.cpp
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */

#include "tiny_sync_mq.h"


tiny_sync_mq::~tiny_sync_mq() {
    
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
    auto e = msgPool_[chanId].tq;
    if (e == msgPool_.end())
        return -1;
    e.put(msg);
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
std::unique_ptr<Msg> tiny_sync_mq::get(int chan, int millisec) {
    auto e = msgPool_[chanId].tq;
    if (e == msgPool_.end())
        return nullptr;
    return e.get(msg);
}

