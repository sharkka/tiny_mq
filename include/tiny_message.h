/**
 * Copyright (C) <2018> <copyright Horizon Robotics>
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction,  
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,  
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is  
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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
