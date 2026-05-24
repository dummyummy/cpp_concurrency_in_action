#pragma once

#include <algorithm>
#include <future>
#include <list>
#include <thread>
#include <type_traits>
#include <utility>

template <typename F, typename A> auto spawn_task(F &&f, A &&a) -> std::future<typename std::invoke_result_t<F, A>>
{
    typedef std::invoke_result_t<F, A> result_type;
    typedef std::decay_t<A> arg_type;
    std::packaged_task<result_type(arg_type)> task(std::forward<F>(f));
    std::future<result_type> res = task.get_future();
    std::thread t(std::move(task), std::forward<A>(a));
    t.detach();
    return res;
}

template <typename T> std::list<T> parallel_quick_sort(std::list<T> input)
{
    if (input.empty())
        return input;

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const &pivot = *result.begin();
    auto divide_point = std::partition(input.begin(), input.end(), [&](T const &t) { return t < pivot; });
    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
    std::future<std::list<T>> new_lower(spawn_task(&parallel_quick_sort<T>, std::move(lower_part)));
    auto new_higher(parallel_quick_sort(std::move(input)));
    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower.get());
    return result;
}