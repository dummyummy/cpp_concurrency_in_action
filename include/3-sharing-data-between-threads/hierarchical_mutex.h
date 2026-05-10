#include <climits>
#include <mutex>
#include <stdexcept>
class hierarchical_mutex
{
  private:
    std::mutex internal_mutex;
    unsigned long const hierarchy_value;
    unsigned long previous_hierarchy_value;
    static thread_local unsigned long this_thread_hierarchy_value;

    void check_for_hierarchy_violation()
    {
        if (this_thread_hierarchy_value <= hierarchy_value)
        {
            throw std::logic_error("mutex hierarchy violated");
        }
    }

    void update_hierarchy_value()
    {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }

  public:
    explicit hierarchical_mutex(unsigned long hierarchy_value)
        : hierarchy_value(hierarchy_value), previous_hierarchy_value(0)
    {
    }

    void lock()
    {
        check_for_hierarchy_violation();
        internal_mutex.lock();
        // 注意必须先lock才能更新value
        update_hierarchy_value();
    }

    void unlock()
    {
        if (this_thread_hierarchy_value != hierarchy_value)
        {
            throw std::logic_error("mutex hierarchy violated");
        }
        // 解锁时必须先更新value再解锁
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }

    bool try_lock()
    {
        check_for_hierarchy_violation();
        if (!internal_mutex.try_lock())
            return false;
        update_hierarchy_value();
        return true;
    }
};

inline thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);
