#include "gtest/gtest.h"
#include <functional>
#include <thread>

static void foo(int &x)
{
    x = 42;
}

class PassArgumentsTestStruct
{
  public:
    void do_lengthy_work()
    {
        x = 42;
    }
    int x = 0;
};

TEST(ThreadManagementTest, PassWithRef)
{
    int x = 0;
    std::thread t(foo, std::ref(x));
    t.join();
    ASSERT_EQ(x, 42);
}

TEST(ThreadManagementTest, PassWithThis)
{
    PassArgumentsTestStruct s;
    ASSERT_EQ(s.x, 0);
    std::thread t(&PassArgumentsTestStruct::do_lengthy_work, &s);
    t.join();
    ASSERT_EQ(s.x, 42);
}