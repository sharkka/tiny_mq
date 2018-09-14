/**
 * @File     tiny_async_mq.h
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */
#ifndef __TINY_ASYNC_MQ_H_
#define __TINY_ASYNC_MQ_H_

/**
 * @Class    tiny_async_mq
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
class tiny_async_mq {
public:
    tiny_async_mq() = delete;
    tiny_async_mq(const tiny_async_mq&) = delete;
    tiny_async_mq& operator=(const tiny_async_mq&) = delete;
    virtual ~tiny_async_mq();

    int    subscribe(int chan) override;
    int    subscribe(int chan, UserCallback userCallback) override;
    int    unsubscribe(int chan) override;
    int    registerEvent(int chan, UserCallback userCallback) override;
    int    publish(int chan) override;
    int    start() override;
    int    stop() override;

protected:
    int    pushSubscriber(int chan, UserCallback userCallback);
    int    popSubscriber(int chan);
};

#endif
