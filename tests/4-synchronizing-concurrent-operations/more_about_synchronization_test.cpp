#include "gtest/gtest.h"
#include "parallel_quicksort.h"

TEST(UseSynchronizationToSimplify, SpawnTask)
{
    int x = 0;
    auto res = spawn_task([](int &x) { x += 1; }, std::ref(x));
    res.get();
    EXPECT_EQ(x, 1);
}

// 1. 常规场景：乱序且包含重复元素的列表
TEST(ParallelQuickSortTest, BasicUnorderedSort)
{
    std::list<int> input = {5, 2, 9, 1, 5, 6, 3};
    std::list<int> expected = {1, 2, 3, 5, 5, 6, 9};
    
    auto result = parallel_quick_sort(input);
    
    // std::list 重载了 operator==，可以直接使用 EXPECT_EQ 进行逐项对比
    EXPECT_EQ(result, expected);
}

// 2. 边界条件：空列表
TEST(ParallelQuickSortTest, EmptyList)
{
    std::list<int> input = {};
    std::list<int> expected = {};
    
    auto result = parallel_quick_sort(input);
    
    EXPECT_TRUE(result.empty());
    EXPECT_EQ(result, expected);
}

// 3. 边界条件：只有一个元素的列表
TEST(ParallelQuickSortTest, SingleElement)
{
    std::list<int> input = {42};
    std::list<int> expected = {42};
    
    auto result = parallel_quick_sort(input);
    
    EXPECT_EQ(result, expected);
}

// 4. 特殊场景：已经完全正序排好序的列表
TEST(ParallelQuickSortTest, AlreadySorted)
{
    std::list<int> input = {1, 2, 3, 4, 5, 6, 7};
    std::list<int> expected = {1, 2, 3, 4, 5, 6, 7};
    
    auto result = parallel_quick_sort(input);
    
    EXPECT_EQ(result, expected);
}

// 5. 特殊场景：完全逆序的列表
TEST(ParallelQuickSortTest, ReverseSorted)
{
    std::list<int> input = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    std::list<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    auto result = parallel_quick_sort(input);
    
    EXPECT_EQ(result, expected);
}

// 6. 进阶场景：更长的数据序列测试 (借助标准库验证)
TEST(ParallelQuickSortTest, LargeRandomList)
{
    std::list<int> input = {45, 12, 89, 33, 71, 2, 99, 100, 3, 56, 23, 77, 8, 19};
    
    auto result = parallel_quick_sort(input);
    
    // 直接使用标准库算法 std::is_sorted 来验证结果是否为升序
    EXPECT_TRUE(std::is_sorted(result.begin(), result.end()));
    
    // 确保排序后没有丢失或多出元素
    EXPECT_EQ(result.size(), input.size()); 
}