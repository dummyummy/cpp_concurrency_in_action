#pragma once

#include <stdexcept>
#include <thread>

namespace CppConcurrency
{
class scoped_thread
{
public:
    // t_ is an lvalue
    explicit scoped_thread(std::thread t_) : t(std::move(t_))
    {
        if (!t.joinable())
            throw std::logic_error("Thread is not joinable");
    }

    ~scoped_thread()
    {
        // t may be moved already
        if (t.joinable())
            t.join();
    }

    scoped_thread(const scoped_thread &) = delete;
    scoped_thread &operator=(const scoped_thread &) = delete;
    scoped_thread(scoped_thread&&) noexcept = default;
    scoped_thread& operator=(scoped_thread&&) noexcept = default;

private:
    std::thread t;
};
} // namespace CppConcurrency