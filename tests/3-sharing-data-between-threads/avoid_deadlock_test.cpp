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
        std::lock(a.m, b.m);
        std::lock_guard lock_a(a.m, std::adopt_lock);
        std::lock_guard lock_b(b.m, std::adopt_lock);
        std::swap(a.value, b.value);
    }

    friend void scoped_swap(X &a, X &b)
    {
        if (&a == &b)
            return;
        std::scoped_lock guard(a.m, b.m);
        std::swap(a.value, b.value);
    }
};

TEST(AvoidDeadlockTest, StdLock)
{
    X a(1);
    X b(2);
    swap(a, b);
    ASSERT_EQ(a.value, 2);
    ASSERT_EQ(b.value, 1);
}

TEST(AvoidDeadlockTest, ScopedSwap)
{
    X a(1);
    X b(2);
    scoped_swap(a, b);
    ASSERT_EQ(a.value, 2);
    ASSERT_EQ(b.value, 1);
}