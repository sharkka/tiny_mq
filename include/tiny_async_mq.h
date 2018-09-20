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
 * @File     tiny_async_mq.h
 * @Brief
 * @DateTime 2018-09-13T18:22:31+0800
 * @Modify   2018-09-13T18:22:31+0800
 * @Author   Anyz
 */
#ifndef __TINY_ASYNC_MQ_H_
#define __TINY_ASYNC_MQ_H_

#include "tiny_mq.h"

/**
 * @Class    tiny_async_mq
 * @Brief
 * @DateTime 2018/9/13 18:23:50
 * @Modify   2018/9/13 18:23:55
 * @Author   Anyz
 */
class tiny_async_mq : public tiny_mq {
public:
    tiny_async_mq(const tiny_async_mq&) = delete;
    tiny_async_mq& operator=(const tiny_async_mq&) = delete;
    static tiny_mq* getInstance();
    virtual ~tiny_async_mq();

    int    subscribe(int chanId) override;
    int    subscribe(int chanId, UserCallback userCallback) override;
    int    unsubscribe(int chanId) override;
    int    registerEvent(int chanId, UserCallback userCallback) override;
    int    publish(tiny_complex_queue* complexQueue) override;
    int    put(int chanId, TinyMsg&& msg) override;
    int    put(int chanId, tiny_message* msg) override;
    int    start() override;
    int    stop() override;
    void   clean();
    static void   destroy();

protected:
    int    pushSubscriber(int chanId, UserCallback userCallback);
    int    popSubscriber(int chanId);

private:
    tiny_async_mq() {}
};

#endif
