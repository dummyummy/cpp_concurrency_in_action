#include "gtest/gtest.h"
#include <atomic>
#include <thread>
#include <vector>

#include "threadsafe_queue.h"

using namespace CppConcurrency;

// ==========================================
// 1. 单线程基础功能测试
// ==========================================
TEST(ThreadSafeQueueTest, SingleThreadBasicOperations) {
    threadsafe_queue<int> q;

    // 初始状态
    EXPECT_TRUE(q.empty());

    int value = 0;
    // 空队列 try_pop 测试
    EXPECT_FALSE(q.try_pop(value));
    EXPECT_EQ(q.try_pop(), nullptr);

    // 压入数据
    q.push(10);
    q.push(20);
    q.push(30);
    EXPECT_FALSE(q.empty());

    // 传引用 try_pop 测试
    EXPECT_TRUE(q.try_pop(value));
    EXPECT_EQ(value, 10);

    // 返回 shared_ptr 的 try_pop 测试
    auto ptr = q.try_pop();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 20);

    // wait_and_pop 测试 (因为队列非空，此时不会阻塞)
    q.wait_and_pop(value);
    EXPECT_EQ(value, 30);

    // 队列应再次为空
    EXPECT_TRUE(q.empty());
}

// ==========================================
// 2. 阻塞与唤醒机制测试
// ==========================================
TEST(ThreadSafeQueueTest, WaitAndPopBlocksUntilPushed) {
    threadsafe_queue<int> q;
    std::atomic<bool> consumer_started{false};
    std::atomic<bool> popped{false};
    int popped_value = 0;

    // 消费者线程
    std::thread consumer([&]() {
        consumer_started = true;
        q.wait_and_pop(popped_value); // 此时队列为空，应该阻塞在这里
        popped = true;
    });

    // 确保消费者已经启动并进入等待状态
    while (!consumer_started.load()) {
        std::this_thread::yield();
    }
    
    // 主线程稍微休眠，断言消费者确实被阻塞了（没有 pop 出数据）
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(popped.load());

    // 生产者 push 数据，唤醒消费者
    q.push(42);

    // 等待消费者完成
    consumer.join();

    EXPECT_TRUE(popped.load());
    EXPECT_EQ(popped_value, 42);
}

// ==========================================
// 3. 高并发压力测试 (配合 TSan 使用的最佳用例)
// ==========================================
TEST(ThreadSafeQueueTest, HighContentionConcurrentAccess) {
    threadsafe_queue<int> q;
    
    const int num_producers = 100;
    const int num_consumers = 100;
    const int items_per_thread = 5000;
    
    std::atomic<bool> start_flag{false};
    std::atomic<long long> total_popped_sum{0};

    // 生产者任务：疯狂 push
    auto producer_task = [&]() {
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield(); // 等待发令枪
        }
        for (int i = 1; i <= items_per_thread; ++i) {
            q.push(i);
        }
    };

    // 消费者任务：疯狂 pop
    auto consumer_task = [&]() {
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield(); // 等待发令枪
        }
        
        long long local_sum = 0;
        for (int i = 0; i < items_per_thread; ++i) {
            int value;
            // 因为生产者和消费者数量相同，且每个线程处理的数量相同
            // wait_and_pop 最终一定能拿到足够的数据，不会死锁
            q.wait_and_pop(value); 
            local_sum += value;
        }
        // 累加到全局结果中
        total_popped_sum.fetch_add(local_sum, std::memory_order_relaxed);
    };

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // 创建线程
    for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back(producer_task);
    }
    for (int i = 0; i < num_consumers; ++i) {
        consumers.emplace_back(consumer_task);
    }

    // 所有线程就位，开枪！
    start_flag.store(true, std::memory_order_release);

    // 等待所有线程执行完毕
    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    // 验证逻辑：
    // 我们 push 了 10 组 [1 到 5000] 的序列。
    // 计算期望的总和：10 * (1 + 5000) * 5000 / 2
    long long expected_single_thread_sum = (1LL + items_per_thread) * items_per_thread / 2;
    long long expected_total_sum = expected_single_thread_sum * num_producers;

    EXPECT_EQ(total_popped_sum.load(std::memory_order_acquire), expected_total_sum);
    EXPECT_TRUE(q.empty());
}