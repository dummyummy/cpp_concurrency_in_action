#include <chrono>
#include <thread>

#include "gtest/gtest.h"

struct func
{
    int &i;
    func(int &i_) : i(i_)
    {
    }
    void operator()()
    {
        for (unsigned j = 0; j < 400000000; ++j)
        {
            i++;
        }
    }
};

void trigger_bug()
{
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    my_thread.detach();
}

TEST(ThreadManagementTest, StackUseAfterScope)
{
    ASSERT_DEATH(
    {
        trigger_bug();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }, "AddressSanitizer");
}