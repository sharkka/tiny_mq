/**
 * @File     tiny_mq.h
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */
#ifndef __TINY_MQ_H_
#define __TINY_MQ_H_

#include <mutex>
#include <map>

#include "tiny_queue.h"
#include "Msg.hpp"

//#define __ST static

typedef void (*UserCallback)(uint64_t chanId, Msg* userData);
/**
 * @Struct   tiny_msg
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
typedef struct_tiny_msg {
    int           chanId;
    int           userId;
    int           msgId;
    int           status;
    userCallback  userCallback;
} tiny_msg;

typedef struct_tiny_async_queue {
    tiny_queue*   tq;
    userCallback  userCallback;
    std::thread   queueThread;
    bool          running;
} tiny_async_queue;

/**
 * @Class    tiny_mq
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
class tiny_mq {
public:
    using TinyMsgPool  = std::map<uint64_t, tiny_async_queue>;
    using TinyMsg      = Msg;
public:
    tiny_mq() = delete;
    tiny_mq(const tiny_mq&) = delete;
    tiny_mq& operator=(const tiny_mq&) = delete;
    virtual ~tiny_mq();
    static tiny_mq*  getInstance();

    virtual int    subscribe(uint64_t chan) = 0;
    virtual int    subscribe(uint64_t chan, UserCallback userCallback) = 0;
    virtual int    unsubscribe(uint64_t chan) = 0;
    virtual int    registerEvent(uint64_t chan, UserCallback userCallback) = 0;
    virtual int    publish(uint64_t chan) = 0;
    virtual int    put(TinyMsg&& msg) = 0;
    virtual std::unique_ptr<Msg> get(uint64_t chan, int millisec = 0) = 0;

    virtual int    start() = 0;
    virtual int    stop() = 0;

    uint64_t       createChannel(int maxMqSize = 64);
    int            destroyChannel(uint64_t chanId);
    void           setMaxChannels(int maxCh);
    void           setMaxMqSize(uint64_t chanId, int size);

    void           debugOn();
    void           debugOff();
    void           dumpMqStatus();
    TinyMsgPool*   poolHandle() const {return &msgPool_;}

protected:
    TinyMsgPool    msgPool_;
    std::mutex     instanceMtx_;
    std::mutex     poolMtx_;
    static tiny_mq*  tmq_ = nullptr;

private:
    int            generateChannelId();
    bool           debugFlag_   = false;
    int            maxChannels_ = 32;
};

#endif
