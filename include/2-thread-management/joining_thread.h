#pragma once

#include <thread>
#include <utility>

namespace CppConcurrency
{
class joining_thread
{
  public:
    joining_thread() = default;
    template <typename Callable, typename... Args>
    explicit joining_thread(Callable &&f, Args &&...args) : t(std::forward<Callable>(f), std::forward<Args>(args)...)
    {
    }
    // t_ is an lvalue
    explicit joining_thread(std::thread t_) noexcept : t(std::move(t_))
    {
    }

    joining_thread(joining_thread &&) noexcept = default;

    joining_thread &operator=(joining_thread &&other) noexcept
    {
        if (joinable())
            join();

        t = std::move(other.t);
        return *this;
    }

    joining_thread &operator=(std::thread other) noexcept
    {
        if (joinable())
            join();

        t = std::move(other);
        return *this;
    }

    ~joining_thread()
    {
        if (joinable())
            join();
    }

    joining_thread(const joining_thread &) = delete;
    joining_thread &operator=(const joining_thread &) = delete;

    void swap(joining_thread &other) noexcept
    {
        t.swap(other.t);
    }

    std::thread::id get_id() const noexcept
    {
        return t.get_id();
    }

    bool joinable() const noexcept
    {
        return t.joinable();
    }

    void join()
    {
        t.join();
    }

    void detach()
    {
        t.detach();
    }

    std::thread& as_thread() noexcept
    {
        return t;
    }

    const std::thread& as_thread() const noexcept
    {
        return t;
    }

  private:
    std::thread t;
};
} // namespace CppConcurrency