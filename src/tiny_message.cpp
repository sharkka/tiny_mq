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

int tiny_message::getMsgId() const {
    return msgId_;
}

MsgUID tiny_message::getUniqueId() const {
    return uniqueId_;
}

