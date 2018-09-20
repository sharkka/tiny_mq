#ifndef __TINY_MESSAGE_HPP
#define __TINY_MESSAGE_HPP

#include <memory>
#include <utility>
#include <mutex>


using MsgUID = unsigned long long;

class tiny_message {
public:
    tiny_message(int msgId);
    virtual ~tiny_message() = default;
    virtual std::unique_ptr<tiny_message> move();

    int getMsgId() const;
    MsgUID getUniqueId() const;

    void setRef(int n) {std::lock_guard<std::mutex> lck(*mtx_); refCount_ = n;}
    void incRef() {std::lock_guard<std::mutex> lck(*mtx_); refCount_ ++;}
    void decRef() {std::lock_guard<std::mutex> lck(*mtx_); refCount_ --;}
    int  refCount() const {return refCount_;}

protected:
    tiny_message(tiny_message&&) = default;
    tiny_message& operator=(tiny_message&&) = default;
    int msgId_;
    MsgUID uniqueId_;

private:
    int    refCount_ = 0;
    std::mutex* mtx_;
};

template <typename PayloadType>
class DataMsg : public tiny_message {
public:
    template <typename ... Args>
    DataMsg(int msgId, Args&& ... args)
      : tiny_message(msgId),
        pl_(new PayloadType(std::forward<Args>(args) ...)) {
    }

    virtual ~DataMsg() = default;
    DataMsg(const DataMsg& dataMsg)=delete;
    DataMsg& operator=(const DataMsg& dataMsg)=delete;
    static DataMsg* newDataMsg(int chanId, PayloadType& payload) {
        DataMsg<PayloadType>* msgData = new DataMsg<PayloadType>(chanId, payload);
        return msgData;
    }
    static DataMsg* newDataMsg(int chanId, PayloadType&& payload) {
        DataMsg<PayloadType>* msgData = new DataMsg<PayloadType>(chanId, payload);
        return msgData;
    }

    virtual std::unique_ptr<tiny_message> move() override {
        return std::unique_ptr<tiny_message>(new DataMsg<PayloadType>(std::move(*this)));
    }

    PayloadType& getPayload() const {
        return *pl_;
    }

protected:
    DataMsg(DataMsg&&) = default;
    DataMsg& operator=(DataMsg&&) = default;

private:
    std::shared_ptr<PayloadType> pl_;
};

#endif
