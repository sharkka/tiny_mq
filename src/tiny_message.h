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
    tiny_message(const tiny_message& msg) {this->clone(msg);}
    tiny_message& operator=(const tiny_message& msg) {return this->clone(msg);}

    virtual std::unique_ptr<tiny_message> move();
    virtual std::shared_ptr<tiny_message> copy();
    virtual tiny_message& clone(const tiny_message& msg);

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
        pl_(new PayloadType(std::forward<Args>(args) ...))
    {
    }

    virtual ~DataMsg() = default;
    DataMsg(const DataMsg& dataMsg) : tiny_message(dataMsg) {
        this->clone(dataMsg);
    }
    DataMsg& operator=(const DataMsg& dataMsg) {
        if (this == &dataMsg)
            return *this;
        tiny_message::operator=(dataMsg);
        this->clone(dataMsg);
        return *this;
    }

    virtual std::unique_ptr<tiny_message> move() override {
        return std::unique_ptr<tiny_message>(new DataMsg<PayloadType>(std::move(*this)));
    }
    std::shared_ptr<tiny_message> copy() override {
        std::shared_ptr<tiny_message> smsg(new DataMsg<PayloadType>(*this));
        return smsg;
    }
    DataMsg& clone(const DataMsg& dataMsg) {
        this->msgId_ = dataMsg.getMsgId();
        this->uniqueId_ = dataMsg.getUniqueId();
        this->pl_ = std::shared_ptr<PayloadType>(new PayloadType(dataMsg.getPayload()));
        return *this;
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
