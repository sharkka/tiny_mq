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
 * @File     tiny_sync_mq.h
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */
#ifndef __TINY_SYNC_MQ_H_
#define __TINY_SYNC_MQ_H_

#include <mutex>
#include <map>

#include "tiny_mq.h"
#include "tiny_queue.h"
#include "tiny_message.h"

/**
 * @Class    tiny_sync_mq
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
class tiny_sync_mq : public tiny_mq {
public:
    tiny_sync_mq(const tiny_mq&) = delete;
    tiny_mq& operator=(const tiny_sync_mq&) = delete;
    virtual ~tiny_sync_mq();
    int start() override {return 0;}
    int stop() override {return 0;}

    int    put(int chanId, TinyMsg&& msg) override;
    std::shared_ptr<tiny_message> get(int chanId, int millisec = 0) override;
    static tiny_mq* getInstance();

private:
    tiny_sync_mq() {}
};

#endif
