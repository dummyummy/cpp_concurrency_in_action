#include "gtest/gtest.h"
#include <cstdint>
#include <vector>

#include "parallel_accumulate.h"

TEST(ThreadManagementTest, ParallelAccumulate)
{
    using namespace CppConcurrency;
    std::vector<int64_t> arr(100000000);
    for (size_t i = 0; i < 100000000; i++)
        arr[i] = i + 1;
    auto sum = [](int64_t a, int64_t b) { return (a + b) * (b - a + 1) / 2; };
    ASSERT_EQ(parallel_accumulate(arr.begin(), arr.begin() + 10, (int64_t)0), sum(1, 10));
    ASSERT_EQ(parallel_accumulate(arr.begin(), arr.begin() + 100, (int64_t)0), sum(1, 100));
    ASSERT_EQ(parallel_accumulate(arr.begin(), arr.begin() + 1000, (int64_t)0), sum(1, 1000));
    ASSERT_EQ(parallel_accumulate(arr.begin(), arr.begin() + 10000, (int64_t)0), sum(1, 10000));
    ASSERT_EQ(parallel_accumulate(arr.begin(), arr.begin() + 1000000, (int64_t)0), sum(1, 1000000));
    ASSERT_EQ(parallel_accumulate(arr.begin(), arr.begin() + 10000000, (int64_t)0), sum(1, 10000000));
    ASSERT_EQ(parallel_accumulate(arr.begin(), arr.begin() + 100000000, (int64_t)0), sum(1, 100000000));
}