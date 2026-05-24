#include "gtest/gtest.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

TEST(FutureTest, BasicUsage)
{
    std::shared_ptr<int> spx = std::make_shared<int>(10);
    std::future<int> ft = std::async([p = spx] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return (*p) * (*p);
    });
    auto t1 = std::chrono::steady_clock::now();
    auto res = ft.get();
    auto t2 = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    EXPECT_EQ(res, 100);
    GTEST_LOG_(INFO) << "async sqr lasts " << elapsed.count() << " ms" << std::endl;
    EXPECT_GT(elapsed.count(), 500);
}

TEST(FutureTest, PackagedTask)
{
    using ft = int(int);
    std::mutex m;
    std::condition_variable cv;
    std::queue<std::packaged_task<ft>> tasks;
    std::vector<std::future<int>> futures;
    std::atomic_bool end_flag(false);

    std::thread p(
        [](std::mutex &m, std::condition_variable &cv, std::queue<std::packaged_task<ft>> &tasks,
           std::atomic_bool &end_flag) {
            int i = 0;
            while (!end_flag.load())
            {
                std::packaged_task<ft> task;
                {
                    std::unique_lock lk(m);
                    cv.wait(lk, [&] { return !tasks.empty(); });
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task(i++);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        },
        std::ref(m), std::ref(cv), std::ref(tasks), std::ref(end_flag));

    futures.resize(10);
    for (int i = 0; i < 10; i++)
    {
        std::packaged_task<ft> task([](int x) { return x * x; });
        futures[i] = task.get_future();
        std::lock_guard lk(m);
        tasks.push(std::move(task));
        cv.notify_one();
    }

    for (int i = 0; i < 10; i++)
    {
        auto res = futures[i].get();
        EXPECT_EQ(res, i * i);
    }

    end_flag.store(true);
    p.join();
}

TEST(FutureTest, Promise)
{
    // 1. 创建 promise 和对应的 future
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    // 2. 启动后台线程干活
    std::thread worker([&prom]() {
        // 模拟一点耗时操作
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // 填入数据，兑现承诺！
        prom.set_value(42); 
    });

    // 3. 主线程（测试线程）直接阻塞拿结果
    // fut.get() 会死等，直到 worker 线程调用了 set_value
    int result = fut.get();

    // 4. GTest 断言：验证拿到的是不是 42
    EXPECT_EQ(result, 42);

    // 别忘了回收线程
    worker.join();
}

TEST(FutureTest, FutureException)
{
    std::promise<int> prom;
    prom.set_exception(std::make_exception_ptr(std::logic_error("Exception in a future")));
    EXPECT_THROW(prom.get_future().get(), std::logic_error);
}

TEST(FutureTest, WaitForDuration)
{
    using namespace std::chrono_literals;

    std::promise<int> prom;
    auto ft = prom.get_future();
    EXPECT_TRUE(ft.valid());
    auto t1 = std::thread([&prom] {
        std::this_thread::sleep_for(500ms);
        prom.set_value(42);
    });
    auto wait_res = ft.wait_for(50ms);

    EXPECT_NE(wait_res, std::future_status::ready);

    t1.join();
}