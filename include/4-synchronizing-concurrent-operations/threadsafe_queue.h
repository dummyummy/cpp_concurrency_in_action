#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace CppConcurrency
{
template <typename T> class threadsafe_queue
{
  private:
    mutable std::mutex mtx;
    std::queue<T> data;
    std::condition_variable cv;

  public:
    threadsafe_queue()
    {
    }
    threadsafe_queue(threadsafe_queue const &other)
    {
        std::lock_guard lk(other.mtx);
        data = other.data;
    }
    // 没有必要！
    threadsafe_queue(threadsafe_queue &&) = delete;
    threadsafe_queue<T> &operator=(threadsafe_queue const&) = delete;
    threadsafe_queue<T> &operator=(threadsafe_queue &&) = delete;

    bool empty() const
    {
        std::lock_guard lk(mtx);
        return data.empty();
    }

    void push(T new_value)
    {
        std::lock_guard lk(mtx);
        data.push(new_value);
        cv.notify_one();
    }

    void wait_and_pop(T &value)
    {
        std::unique_lock lk(mtx);
        cv.wait(lk, [this]() { return !data.empty(); });
        value = data.front();
        data.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock lk(mtx);
        cv.wait(lk, [this]() { return !data.empty(); });
        std::shared_ptr<T> ret(std::make_shared<T>(data.front()));
        data.pop();
        return ret;
    }

    bool try_pop(T &value)
    {
        std::unique_lock lk(mtx);
        if (data.empty())
            return false;
        value = data.front();
        data.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::unique_lock lk(mtx);
        if (data.empty())
            return nullptr;
        std::shared_ptr<T> ret(std::make_shared<T>(data.front()));
        data.pop();
        return ret;
    }
};
} // namespace CppConcurrency