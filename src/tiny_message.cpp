#include "tiny_message.h"

#include <atomic>

MsgUID generateUniqueId() {
    static std::atomic<MsgUID> i(0);
    return ++i;
}

tiny_message::tiny_message(int msgId)
  : msgId_(msgId), uniqueId_(generateUniqueId()) {
    mtx_ = new std::mutex;
}

std::unique_ptr<tiny_message> tiny_message::move() {
    return std::unique_ptr<tiny_message>(new tiny_message(std::move(*this)));
}

std::shared_ptr<tiny_message> tiny_message::copy() {
    return std::shared_ptr<tiny_message>(new tiny_message(msgId_));
}

tiny_message& tiny_message::clone(const tiny_message& msg) {
    this->msgId_ = msg.getMsgId();
    this->uniqueId_ = msg.getUniqueId();
    //
    return *this;
}

int tiny_message::getMsgId() const {
    return msgId_;
}

MsgUID tiny_message::getUniqueId() const {
    return uniqueId_;
}

