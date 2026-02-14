#include "gtest/gtest.h"
#include <thread>

#include "thread_guard.h"

TEST(ThreadManagementTest, NoThreadGuard)
{
    ASSERT_DEATH({
        using namespace CppConcurrency;
        std::thread t([](){});
    }, "");
}

TEST(ThreadManagementTest, ThreadGuard)
{
    using namespace CppConcurrency;
    std::thread t([](){});
    thread_guard tg(t);
}