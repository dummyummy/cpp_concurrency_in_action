#include "gtest/gtest.h"
#include <thread>
#include <type_traits>

#include "joining_thread.h"

TEST(ThreadManagementTest, JoiningThread)
{
    using namespace CppConcurrency;
    joining_thread jt1;
    ASSERT_FALSE(jt1.joinable());
    jt1 = joining_thread(std::thread([](){}));
    jt1 = joining_thread([](){});
    ASSERT_FALSE(std::is_function_v<joining_thread>);
    auto tid1 = jt1.get_id();
    joining_thread jt2(std::move(jt1));
    ASSERT_FALSE(jt1.joinable());
    ASSERT_TRUE(jt2.joinable());
    ASSERT_EQ(tid1, jt2.get_id());
    jt1 = std::thread([](){});
    tid1 = jt1.get_id();
    auto tid2 = jt2.get_id();
    jt1.swap(jt2);
    ASSERT_TRUE(jt1.joinable());
    ASSERT_TRUE(jt2.joinable());
    ASSERT_EQ(tid1, jt2.get_id());
    ASSERT_EQ(tid2, jt1.get_id());
}