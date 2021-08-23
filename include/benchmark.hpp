#pragma once

namespace Performance{
    
    namespace benchmark{
        long fibonacci(unsigned n)
        {
            if (n < 2) return n;
            return fibonacci(n-1) + fibonacci(n-2);
        }
    }
}