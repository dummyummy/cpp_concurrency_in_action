#include "gtest/gtest.h"
#include <stdexcept>
#include <thread>

#include "hierarchical_mutex.h"

TEST(HierarchicalMutexTest, BasicLockUnlock) {
    hierarchical_mutex m(1000);
    
    // 基本的锁定和解锁操作
    m.lock();
    m.unlock();
    
    // 多次锁定和解锁
    for (int i = 0; i < 10; ++i) {
        m.lock();
        m.unlock();
    }
}

TEST(HierarchicalMutexTest, HierarchyViolation) {
    hierarchical_mutex high(1000);
    hierarchical_mutex low(500);
    
    // 先获取低层级锁，再获取高层级锁，应该抛出异常
    low.lock();
    EXPECT_THROW(high.lock(), std::logic_error);
    low.unlock();
    
    // 先获取高层级锁，再获取低层级锁，应该成功
    EXPECT_NO_THROW({
        high.lock();
        low.lock();
        low.unlock();
        high.unlock();
    });
    
}

TEST(HierarchicalMutexTest, UnlockWithoutLock) {
    hierarchical_mutex m(1000);
    
    // 未锁定时解锁，应该抛出异常
    EXPECT_THROW(m.unlock(), std::logic_error);
}

TEST(HierarchicalMutexTest, TryLockSuccess) {
    hierarchical_mutex m(1000);
    
    // 尝试锁定，应该成功
    EXPECT_TRUE(m.try_lock());
    m.unlock();
}

TEST(HierarchicalMutexTest, UnlockViolation) {
    hierarchical_mutex m1(1000);
    hierarchical_mutex m2(500);
    
    EXPECT_NO_THROW(m1.lock());
    EXPECT_NO_THROW(m2.lock());

    EXPECT_THROW(m1.unlock(), std::logic_error);

    m2.unlock();
    m1.unlock();
}

TEST(HierarchicalMutexTest, TryLockHierarchyViolation) {
    hierarchical_mutex high(1000);
    hierarchical_mutex low(500);
    
    // 先获取低层级锁，再尝试获取高层级锁，应该抛出异常
    low.lock();
    EXPECT_THROW(high.try_lock(), std::logic_error);
    low.unlock();
}

TEST(HierarchicalMutexTest, MultipleThreads) {
    hierarchical_mutex high(1000);
    hierarchical_mutex medium(750);
    hierarchical_mutex low(500);
    
    for (int i = 0; i < 10000; i++)
    {
        // 测试多线程下的层级检查
        std::thread t1([&]() {
            high.lock();
            medium.lock();
            low.lock();
            
            low.unlock();
            medium.unlock();
            high.unlock();
        });
        
        std::thread t2([&]() {
            medium.lock();
            low.lock();
            
            low.unlock();
            medium.unlock();
        });

        t1.join();
        t2.join();
    }
}

TEST(HierarchicalMutexTest, ThreadLocalHierarchyValue) {
    hierarchical_mutex m1(1000);
    hierarchical_mutex m2(500);


    for (int i = 0; i < 10000; i++)
    {
        // 测试线程本地层级值的独立性
        std::thread t1([&]() {
            m1.lock();
            
            // 在t1中，应该可以获取更低层级的锁
            EXPECT_NO_THROW(m2.lock());
            
            m2.unlock();
            m1.unlock();
        });
        
        std::thread t2([&]() {
            // 在t2中，应该可以独立获取锁
            m2.lock();
            EXPECT_THROW(m1.lock(), std::logic_error);
            m2.unlock();
        });
        
        t1.join();
        t2.join();
    }
}