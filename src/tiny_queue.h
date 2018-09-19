#ifndef __TINY_QUEUE_H_
#define __TINY_QUEUE_H_

#include "tiny_message.h"
#include <memory>
#include <mutex>

const int MSG_TIMEOUT = -1;


class tiny_queue {
public:
    using LockGuard = std::lock_guard<std::mutex>;
public:
    tiny_queue();
    explicit tiny_queue(uint64_t ch, int maxSize = -1);
    ~tiny_queue();

    void put(tiny_message&& msg);
    void put(tiny_message& msg);
    void put(tiny_message* msg);
    std::shared_ptr<tiny_message> get(int timeoutMillis = 0);
    std::shared_ptr<tiny_message> request(tiny_message&& msg);

    void      respondTo(MsgUID reqUid, tiny_message&& responseMsg);
    void      setChannel(uint64_t ch) { chanId_ = ch; }
    void      setMaxSize(int maxSize) { maxSize_ = maxSize; }
    uint64_t  channelId() const {return chanId_;}
    size_t    size() const;

private:
    class Impl;
    std::shared_ptr<Impl> impl_;
    uint64_t chanId_;
    int maxSize_;
};

#endif

