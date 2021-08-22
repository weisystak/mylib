#pragma once

namespace Performance{
    // utility
    template<typename T> //  TD declared only for printing deduced type
    class TD; 
    
    namespace benchmark{
        long fibonacci(unsigned n)
        {
            if (n < 2) return n;
            return fibonacci(n-1) + fibonacci(n-2);
        }
    }
}