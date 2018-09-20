/**
 * @File     tiny_sync_mq.h
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */
#ifndef __TINY_SYNC_MQ_H_
#define __TINY_SYNC_MQ_H_

#include <mutex>
#include <map>

#include "tiny_mq.h"
#include "tiny_queue.h"
#include "tiny_message.h"

/**
 * @Class    tiny_sync_mq
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
class tiny_sync_mq : public tiny_mq {
public:
    tiny_sync_mq(const tiny_mq&) = delete;
    tiny_mq& operator=(const tiny_sync_mq&) = delete;
    virtual ~tiny_sync_mq();
    int start() override {return 0;}
    int stop() override {return 0;}

    int    put(int chanId, TinyMsg&& msg) override;
    std::shared_ptr<tiny_message> get(int chanId, int millisec = 0) override;
    static tiny_mq* getInstance();

private:
    tiny_sync_mq() {}
};

#endif
