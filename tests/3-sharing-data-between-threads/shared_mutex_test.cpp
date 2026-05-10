#include "gtest/gtest.h"
#include <map>
#include <mutex>
#include <shared_mutex>

template <typename T> class ReadWriteCache
{
    std::map<std::string, T> data;
    mutable std::shared_mutex entry_mutex;

  public:
    T get(const std::string &key) const
    {
        std::shared_lock lk(entry_mutex); // shared read lock
        typename std::map<std::string, T>::const_iterator it = data.find(key);
        return it == data.end() ? T() : it->second;
    }

    void set(std::string const &key, T const &value)
    {
        std::lock_guard lk(entry_mutex); // exclusive write lock
        data[key] = value;
    }
};

TEST(SharedMutexTest, BasicUsage)
{
    // 使用shared_lock获取shared access
    // 使用lock_guard或unique_lock获取exclusive access
}