#include "timer.hpp"
#include <cmath>

long fibonacci(unsigned n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int main()
{
    // example 1
    Timer timer;
    timer.start();
    int counter = 0;
    double test, test2;
    while(timer.elapsedSeconds() < 0.01)
    {
        counter++;
        test = std::cos(counter / M_PI);
        test2 = std::sin(counter / M_PI);
    }
    timer.stop();
    
    std::cout << counter << std::endl;
    std::cout << "Seconds: " << timer.elapsedSeconds() << std::endl;
    std::cout << "Milliseconds: " << timer.elapsedMilliseconds() << std::endl;

    // example 2
    {
        timer_guard tg(__LINE__);
        fibonacci(20);
    }

    // example 3
    using microseconds = std::chrono::duration<double, std::micro>; // or replace std::micro with std::ratio<1, 1'000'000>
    auto dur = new std::chrono::steady_clock::duration;
    {
        timer_guard tg(dur);
        fibonacci(20);
    }
    std::cout << microseconds(*dur).count() << " [µs]"<< std::endl;  // 71.448 [µs]
    std::cout<< std::chrono::duration_cast<std::chrono::nanoseconds>(*dur).count() / 1'000.0 << std::endl; // 71.448
    std::cout<< std::chrono::duration_cast<std::chrono::microseconds>(*dur).count() << std::endl; // 71 (precision loss)

} 