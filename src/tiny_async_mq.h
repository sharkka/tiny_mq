/**
 * @File     tiny_async_mq.h
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */
#ifndef __TINY_ASYNC_MQ_H_
#define __TINY_ASYNC_MQ_H_

#include "tiny_mq.h"

/**
 * @Class    tiny_async_mq
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
class tiny_async_mq : public tiny_mq {
public:
    tiny_async_mq(const tiny_async_mq&) = delete;
    tiny_async_mq& operator=(const tiny_async_mq&) = delete;
    static tiny_mq* getInstance();
    virtual ~tiny_async_mq();

    int    subscribe(uint64_t chanId) override;
    int    subscribe(uint64_t chanId, UserCallback userCallback) override;
    int    unsubscribe(uint64_t chanId) override;
    int    registerEvent(uint64_t chanId, UserCallback userCallback) override;
    int    publish(tiny_complex_queue* complexQueue) override;
    int    put(uint64_t chanId, TinyMsg&& msg) override;
    int    put(uint64_t chanId, tiny_message* msg) override;
    int    start() override;
    int    stop() override;
    void   clean();
    static void   destroy();

protected:
    int    pushSubscriber(uint64_t chanId, UserCallback userCallback);
    int    popSubscriber(uint64_t chanId);

private:
    tiny_async_mq() {}
};

#endif
