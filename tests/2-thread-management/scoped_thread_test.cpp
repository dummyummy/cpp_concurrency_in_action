#include "gtest/gtest.h"
#include <thread>

#include "scoped_thread.h"

TEST(ThreadManagementTest, ScopedThread)
{
    using namespace CppConcurrency;
    scoped_thread st(std::thread([](){}));
}

TEST(ThreadManagementTest, ScopedThreadWithLValue)
{
    using namespace CppConcurrency;
    std::thread t([](){});
    scoped_thread st1(std::move(t));
    scoped_thread st2(std::move(st1));
    auto st3 = std::move(st2);
}