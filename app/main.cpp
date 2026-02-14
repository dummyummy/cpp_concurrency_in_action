#include <cassert>
#include <iostream>
#include <thread>

void foo(int &counter)
{
    for (int i = 0; i < 10000000; ++i)
        counter++;
}

int main()
{
    int counter = 0;
    std::thread t(foo, std::ref(counter));
    t.join();
    std::cout << "counter: " << counter << std::endl;
    assert(!t.joinable());
    return 0;
}