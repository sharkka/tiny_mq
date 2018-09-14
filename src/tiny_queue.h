#ifndef __TINY_QUEUE_H_
#define __TINY_QUEUE_H_

#include "Msg.hpp"
#include <memory>


const int MSG_TIMEOUT = -1;


class tiny_queue {
public:
    using LockGuard = std::lock_guard<std::mutex>;
public:
    tiny_queue();
    explicit tiny_queue(uint64_t ch, int maxSize = -1);
    ~tiny_queue();

    void put(Msg&& msg);
    std::unique_ptr<Msg> get(int timeoutMillis = 0);
    std::unique_ptr<Msg> request(Msg&& msg);

    void      respondTo(MsgUID reqUid, Msg&& responseMsg);
    void      setChannel(uint64_t ch);
    void      setMaxSize(int maxSize);
    uint64_t  channelId() const {return chanId_;}

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    uint64_t chanId_;
    int maxSize_;
};

#endif

