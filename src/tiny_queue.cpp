#include "tiny_queue.h"

#include <chrono>
#include <condition_variable>
#include <queue>
#include <map>
#include <mutex>
#include <utility>

class tiny_queue::Impl {
public:
    Impl()
      : queue_(), queueMutex_(), queueCond_(), responseMap_(), responseMapMutex_() {
    }
    explicit Impl(int ch, int maxSize)
    : queue_(),
      queueMutex_(),
      queueCond_(),
      responseMap_(),
      responseMapMutex_(),
      chanId_(ch),
      maxSize_(maxSize) {
    } 

    void put(tiny_message&& msg) {
	int timeoutMillis = 0;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (maxSize_ > 0) {
                auto queueFullOccured = !queueFullCond_.wait_for(
                    lock,
                    std::chrono::milliseconds(timeoutMillis),
                    [this]{return queue_.size() < static_cast<unsigned int>(maxSize_);});
                if (queueFullOccured) {
                    printf("WARNING: message queue is full.\n");
                    return;
                }
            }
            queue_.push(msg.move());
        }
        queueCond_.notify_one();
    }

    void put(tiny_message* msg) {
    int timeoutMillis = 0;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (maxSize_ > 0) {
                auto queueFullOccured = !queueFullCond_.wait_for(
                    lock,
                    std::chrono::milliseconds(timeoutMillis),
                    [this]{return queue_.size() < static_cast<unsigned int>(maxSize_);});
                if (queueFullOccured) {
                    printf("WARNING: message queue is full.\n");
                    return;
                }
            }
            auto p = std::shared_ptr<tiny_message>(msg);
            queue_.push(p);
        }
        queueCond_.notify_one();
    }

    std::shared_ptr<tiny_message> get(int timeoutMillis) {
        std::unique_lock<std::mutex> lock(queueMutex_);

        if (timeoutMillis <= 0)
            queueCond_.wait(lock, [this]{return !queue_.empty();});
        else {
            // wait_for returns false if the return is due to timeout
            auto timeoutOccured = !queueCond_.wait_for(
                lock,
                std::chrono::milliseconds(timeoutMillis),
                [this]{return !queue_.empty();});

            if (timeoutOccured)
                queue_.emplace(new tiny_message(MSG_TIMEOUT));
        }
        std::shared_ptr<tiny_message> msg = queue_.front();

        if (1 <= msg->refCount())
            queue_.pop();
        msg->decRef();
        queueFullCond_.notify_one();
        return msg;
    }

    std::shared_ptr<tiny_message> request(tiny_message&& msg) {
        // Construct an ad hoc Queue to handle response Msg
        std::unique_lock<std::mutex> lock(responseMapMutex_);
        auto it = responseMap_.emplace(
            std::make_pair(msg.getUniqueId(), std::shared_ptr<tiny_queue>(new tiny_queue))).first;
        lock.unlock();

        put(std::move(msg));
        auto response = it->second->get(); // Block until response is put to the response Queue

        lock.lock();
        responseMap_.erase(it); // Delete the response Queue
        lock.unlock();

        return response;
    }

    void respondTo(MsgUID reqUid, tiny_message&& responseMsg) {
        std::lock_guard<std::mutex> lock(responseMapMutex_);
        if (responseMap_.count(reqUid) > 0)
            responseMap_[reqUid]->put(std::move(responseMsg));
    }
    size_t size() const {return queue_.size();}

private:
    // Queue for the Msgs
    std::queue<std::shared_ptr<tiny_message>> queue_;
    // Mutex to protect access to the queue
    std::mutex queueMutex_;
    // Condition variable to wait for when getting Msgs from the queue
    std::condition_variable queueCond_;
    // Map to keep track of which response handler queues are associated with which request Msgs
    std::condition_variable queueFullCond_;
    std::map<MsgUID, std::shared_ptr<tiny_queue>> responseMap_;
    // Mutex to protect access to response map
    std::mutex responseMapMutex_;
    int chanId_ = 0;
    int maxSize_ = -1;
};

tiny_queue::tiny_queue() : impl_(new Impl) {}

tiny_queue::tiny_queue(int ch, int maxSize) : impl_(new Impl(ch, maxSize)) {}

tiny_queue::~tiny_queue() {}

void tiny_queue::put(tiny_message&& msg) {
    impl_->put(std::move(msg));
}

void tiny_queue::put(tiny_message* msg) {
    impl_->put(msg);
}

std::shared_ptr<tiny_message> tiny_queue::get(int timeoutMillis) {
    return impl_->get(timeoutMillis);
}

std::shared_ptr<tiny_message> tiny_queue::request(tiny_message&& msg) {
    return impl_->request(std::move(msg));
}

void tiny_queue::respondTo(MsgUID reqUid, tiny_message&& responseMsg) {
    impl_->respondTo(reqUid, std::move(responseMsg));
}

size_t tiny_queue::size() const {
     return impl_->size();
}

