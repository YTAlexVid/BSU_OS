//
// Created by Alex Saharchuk on 26/10/2025.
//

#ifndef BUFFEREDCHANNEL_H
#define BUFFEREDCHANNEL_H
#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) {
        capacity_ = size;
    }

    void send(T task) {
        std::unique_lock lock(mtx_);
        cv_senders_.wait(lock, [this]() { return tasks_.size() < capacity_ || closed_; });
        if (closed_) {
            throw std::runtime_error("Channel is closed!");
        }
        tasks_.push(std::move(task));
        cv_receivers_.notify_one();
    }

    std::pair<T, bool> receive() {
        std::unique_lock lock(mtx_);
        cv_receivers_.wait(lock, [this]() { return !tasks_.empty() || closed_; });
        if (closed_ && tasks_.empty()) {
            return {T(), false};
        }
        T ret = std::move(tasks_.front());
        tasks_.pop();
        cv_senders_.notify_one();
        return {std::move(ret), true};
    }

    void close() {
        std::unique_lock lock(mtx_);
        closed_ = true;
        cv_senders_.notify_all();
        cv_receivers_.notify_all();
    }


private:
    bool closed_ = false;
    std::queue<T> tasks_;
    int capacity_;

    std::mutex mtx_;
    std::condition_variable cv_senders_;
    std::condition_variable cv_receivers_;
};


#endif //BUFFEREDCHANNEL_H
