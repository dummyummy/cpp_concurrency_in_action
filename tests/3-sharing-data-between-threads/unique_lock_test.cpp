#include "gtest/gtest.h"
#include <mutex>

template <typename T> class X
{
  private:
    std::mutex m;

  public:
    T value;

    X(const T &value) : value(value)
    {
    }

    friend void swap(X &a, X &b)
    {
        if (&a == &b)
            return;
        std::unique_lock lock_a(a.m, std::defer_lock);
        std::unique_lock lock_b(b.m, std::defer_lock);
        std::lock(lock_a, lock_b);
        std::swap(a.value, b.value);
    }
};

std::unique_lock<std::mutex> get_lock(std::mutex &m)
{
    std::unique_lock lock(m);
    return lock;
}

TEST(UniqueLockTest, DeferLock) {
    X a(1);
    X b(2);
    swap(a, b);
    ASSERT_EQ(a.value, 2);
    ASSERT_EQ(b.value, 1);
}

TEST(UniqueLockTest, TransferLock) {
    std::mutex m;
    auto lock = get_lock(m);
    ASSERT_TRUE(lock.owns_lock());
}

TEST(UniqueLockTest, LockAndUnlock) {
    std::mutex m;
    auto lock = get_lock(m);
    ASSERT_TRUE(lock.owns_lock());
    lock.unlock();
    ASSERT_FALSE(lock.owns_lock());
    lock.lock();
    ASSERT_TRUE(lock.owns_lock());
}