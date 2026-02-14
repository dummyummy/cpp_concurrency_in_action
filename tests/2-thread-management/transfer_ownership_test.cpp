#include "gtest/gtest.h"
#include <memory>
#include <thread>

#include "thread_guard.h"

static void foo(std::unique_ptr<int> px, void *addr)
{
    *px = 42;
    ASSERT_EQ(addr, px.get());
    ASSERT_EQ(*(reinterpret_cast<int *>(addr)), 42);
}

static void bar()
{
}

static std::thread baz1()
{
    return std::thread(bar);
}

static std::thread baz2()
{
    std::thread t(bar);
    return t;
}

static void boo(std::thread t)
{
    if (t.joinable())
        t.join();
}

TEST(ThreadManagementTest, TransferArgumentOwnership)
{
    auto x = new int(0);
    std::unique_ptr<int> px(x);
    ASSERT_EQ(*x, 0);
    std::thread t(foo, std::move(px), (void *)x);
    t.join();
    ASSERT_EQ(px, nullptr);
}

TEST(ThreadManagementTest, TransferAndAssignOwnership)
{
    using namespace CppConcurrency;

    std::thread t1(bar);
    std::thread t2(std::move(t1));
    ASSERT_FALSE(t1.joinable());
    ASSERT_TRUE(t2.joinable());
    thread_guard tg1(t1);
    thread_guard tg2(t2);
    t1 = std::thread(bar);
}

TEST(ThreadManagementTest, ReassignOwnership)
{
    ASSERT_DEATH({
        using namespace CppConcurrency;
        std::thread t1(bar);
        t1 = std::thread(bar);
    }, "");
}

TEST(ThreadManagementTest, ReturnOwnershipFromFunction)
{
    std::thread t1(baz1());
    std::thread t2(baz2());
    t1.join();
    t2.join();
}

TEST(ThreadManagementTest, TransferOwnershipToFunction)
{
    std::thread t(bar);
    boo(std::move(t));
}