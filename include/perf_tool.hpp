#pragma once
#include <functional>
#include <memory>
#include <thread>
#include "timer.hpp"
#include <vector>

namespace Performance{
    class measure{
    public:
        using microseconds = std::chrono::duration<double, std::micro>;

        measure(std::function<void(int)>&& f, int threads=std::thread::hardware_concurrency(), int repeat=5)
        :f(f),threads(threads), v(threads)
        {
            Timer t;
            for(int i = 0; i < threads; i++)
            {
                timer_guard tg(&v[i]);
                (f)(i+1);
            }

        }
    private:
        std::function<void(int)> f;
        int threads;
        std::vector<std::chrono::steady_clock::duration> v;
    };

    
}