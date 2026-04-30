#include "gtest/gtest.h"
#include <mutex>
#include <thread>
#include <vector>

static int safe_init_counter = 0;
std::once_flag init_flag;

void init()
{
    safe_init_counter++;
}

TEST(SafeInitializationTest, SafeInitialization) {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10000; ++i)
    {
        threads.emplace_back([]() {
            std::call_once(init_flag, init);
        });
    }
    for (auto &t : threads)
    {
        if (t.joinable())
            t.join();
    }
    ASSERT_EQ(safe_init_counter, 1);
}