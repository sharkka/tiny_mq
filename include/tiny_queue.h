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
 * 
 * @File     tiny_queue.cpp
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */
#ifndef __TINY_QUEUE_H_
#define __TINY_QUEUE_H_

#include "tiny_message.h"
#include <memory>
#include <mutex>

const int MSG_TIMEOUT = -1;

/**
 * @Class    tiny_queue
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
class tiny_queue {
public:
    using LockGuard = std::lock_guard<std::mutex>;
public:
    tiny_queue();
    explicit tiny_queue(int ch, int maxSize = -1);
    ~tiny_queue();

    void put(tiny_message&& msg);
    //void put(tiny_message& msg);
    void put(tiny_message* msg);
    std::shared_ptr<tiny_message> get(int timeoutMillis = 0);
    std::shared_ptr<tiny_message> request(tiny_message&& msg);

    void      respondTo(MsgUID reqUid, tiny_message&& responseMsg);
    void      setChannel(int ch) { chanId_ = ch; }
    void      setMaxSize(int maxSize) { maxSize_ = maxSize; }
    int       channelId() const {return chanId_;}
    size_t    size() const;

private:
    class Impl;
    std::shared_ptr<Impl> impl_;
    int chanId_;
    int maxSize_;
};

#endif
