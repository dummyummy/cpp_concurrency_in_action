#pragma once

#include <iterator>
#include <numeric>
#include <thread>
#include <vector>

namespace CppConcurrency
{
template <typename Iterator, typename T> struct accumulate_block
{
    void operator()(Iterator first, Iterator last, T &result)
    {
        result = std::accumulate(first, last, result);
    }
};

template <typename Iterator, typename T>
inline T parallel_accumulate(Iterator first, Iterator last, T init, size_t min_per_thread = 25)
{
    const size_t length = std::distance(first, last);
    if (!length)
        return init;
    const size_t max_threads = (length + min_per_thread - 1) / min_per_thread;
    const size_t hardware_threads = std::thread::hardware_concurrency();
    const size_t num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    const size_t block_size = length / num_threads; // floor
    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);
    Iterator block_start = first;
    for (size_t i = 0; i < num_threads - 1; i++)
    {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(accumulate_block<Iterator, T>(), block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
    for (auto &thread : threads)
        thread.join();
    return std::accumulate(results.begin(), results.end(), init);
}
} // namespace CppConcurrency