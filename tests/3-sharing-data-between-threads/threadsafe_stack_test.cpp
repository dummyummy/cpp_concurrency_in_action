#include "gtest/gtest.h"
#include <thread>
#include <vector>

#include "threadsafe_stack.h"

using namespace CppConcurrency;

TEST(ThreadsafeStackTest, BasicPushPop) {
    threadsafe_stack<int> stack;
    
    // 测试push和pop
    stack.push(1);
    stack.push(2);
    stack.push(3);
    
    int value;
    stack.pop(value);
    EXPECT_EQ(value, 3);
    
    stack.pop(value);
    EXPECT_EQ(value, 2);
    
    stack.pop(value);
    EXPECT_EQ(value, 1);
}

TEST(ThreadsafeStackTest, PopWithSharedPtr) {
    threadsafe_stack<int> stack;
    
    stack.push(42);
    
    auto ptr = stack.pop();
    EXPECT_EQ(*ptr, 42);
}

TEST(ThreadsafeStackTest, EmptyStackException) {
    threadsafe_stack<int> stack;
    
    // 测试空栈时的异常
    EXPECT_THROW(stack.pop(), empty_stack);
    
    int value;
    EXPECT_THROW(stack.pop(value), empty_stack);
}

TEST(ThreadsafeStackTest, EmptyCheck) {
    threadsafe_stack<int> stack;
    
    // 初始状态为空
    EXPECT_TRUE(stack.empty());
    
    // push后不为空
    stack.push(1);
    EXPECT_FALSE(stack.empty());
    
    // pop后为空
    int value;
    stack.pop(value);
    EXPECT_TRUE(stack.empty());
}

TEST(ThreadsafeStackTest, CopyConstructor) {
    threadsafe_stack<int> original;
    original.push(1);
    original.push(2);
    
    // 拷贝构造
    threadsafe_stack<int> copy(original);
    
    // 验证拷贝是否成功
    EXPECT_FALSE(copy.empty());
    
    int value;
    copy.pop(value);
    EXPECT_EQ(value, 2);
    
    copy.pop(value);
    EXPECT_EQ(value, 1);
    
    EXPECT_TRUE(copy.empty());
    // 原栈不受影响
    EXPECT_FALSE(original.empty());
}

TEST(ThreadsafeStackTest, ConcurrentOperations) {
    threadsafe_stack<int> stack;
    const int num_threads = 10;
    const int num_operations = 1000;
    const int total_elements = num_threads * num_operations;
    
    // 多个线程同时push
    std::vector<std::thread> push_threads;
    for (int i = 0; i < num_threads; ++i) {
        push_threads.emplace_back([&stack, i, num_operations]() {
            for (int j = 0; j < num_operations; ++j) {
                stack.push(i * num_operations + j);
            }
        });
    }
    
    for (auto &t : push_threads) {
        t.join();
    }
    
    // 验证栈不为空
    EXPECT_FALSE(stack.empty());
    
    // 计算实际push的元素数量
    int actual_push_count = 0;
    while (!stack.empty()) {
        try {
            stack.pop();
            actual_push_count++;
        } catch (const empty_stack &) {
            break;
        }
    }
    
    // 验证所有元素都被正确push进去
    EXPECT_EQ(actual_push_count, total_elements);
    
    // 再次测试并发push和pop
    threadsafe_stack<int> stack2;
    std::atomic<int> pop_count(0);
    
    // 同时启动push和pop线程
    std::vector<std::thread> mixed_threads;
    
    // push线程
    for (int i = 0; i < num_threads; ++i) {
        mixed_threads.emplace_back([&stack2, i, num_operations]() {
            for (int j = 0; j < num_operations; ++j) {
                stack2.push(i * num_operations + j);
            }
        });
    }
    
    // pop线程
    for (int i = 0; i < num_threads; ++i) {
        mixed_threads.emplace_back([&stack2, &pop_count, num_operations]() {
            for (int j = 0; j < num_operations; ++j) {
                try {
                    stack2.pop();
                    pop_count++;
                } catch (const empty_stack &) {
                    // 忽略空栈异常，因为可能有竞争
                }
            }
        });
    }
    
    for (auto &t : mixed_threads) {
        t.join();
    }
    
    // 清理剩余元素并计数
    while (!stack2.empty()) {
        try {
            stack2.pop();
            pop_count++;
        } catch (const empty_stack &) {
            break;
        }
    }
    
    // 验证最终处理的元素数量正确
    EXPECT_EQ(pop_count, total_elements);
}