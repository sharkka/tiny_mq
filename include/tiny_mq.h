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
#include <vector>
#include <thread>

#include "tiny_queue.h"
#include "tiny_message.h"

typedef void (*UserCallback)(int chanId, tiny_message* userData);
/**
 * @Struct   tiny_msg
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
typedef struct tiny_msg {
    int           chanId;
    int           userId;
    int           msgId;
    int           status;
    UserCallback  userCallback;
} tiny_msg;
/**
 * @Struct   tiny_complex_queue
 * @Brief
 * @DateTime 2018/9/17 15:59:54
 * @Modify   2018/9/17 16:00:02
 * @Author   Anyz
 */
typedef struct _tiny_complex_queue {
    bool          running;
    std::thread*  queueThread = nullptr;
    tiny_queue*   tq = nullptr;
    std::mutex*   mtx = nullptr;
    std::vector<UserCallback>  userCallbacks;
} tiny_complex_queue;

/**
 * @Class    tiny_mq
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
class tiny_mq {
public:
    using TinyMsgPool  = std::map<int, tiny_complex_queue>;
    using TinyMsg      = tiny_message;
public:
    tiny_mq(){}
    tiny_mq(const tiny_mq&) = delete;
    tiny_mq& operator=(const tiny_mq&) = delete;
    virtual ~tiny_mq();

    virtual int    subscribe(int chanId) {return 0;}
    virtual int    subscribe(int chanId, UserCallback userCallback) {return 0;}
    virtual int    unsubscribe(int chanId) {return 0;}
    virtual int    registerEvent(int chanId, UserCallback userCallback) {return 0;}
    virtual int    publish(tiny_complex_queue* complexQueue) {return 0;}
    virtual int    put(int chanId, TinyMsg&& msg) {return 0;}
    virtual int    put(int chanId, TinyMsg* msg) {return 0;}
    virtual std::shared_ptr<TinyMsg> get(int chanId, int millisec = 0) {return nullptr;}

    virtual int    start() {return 0;}
    virtual int    stop() {return 0;}
    virtual void   clean() {}

    int            createChannel(int maxMqSize = 64);
    void           destroyChannel(int chanId);
    void           setMaxChannels(int maxCh);
    void           setMaxMqSize(int chanId, int size);
    TinyMsgPool*   poolHandle() {return &msgPool_;}
    
    class GarbageCollector {
    public:
        ~GarbageCollector() {
            if (tmq_) {
                delete tmq_;
                tmq_ = nullptr;
            }
        }
        static GarbageCollector garbageCollector;
    };
protected:
    TinyMsgPool    msgPool_;
    static std::mutex     instanceMtx_;
    std::mutex     poolMtx_;
    static tiny_mq*  tmq_;
    

private:
    bool           debugFlag_   = false;
    int            maxChannels_ = 32;

};

#define AutoCollector() tiny_mq::GarbageCollector garbageCollector

#endif