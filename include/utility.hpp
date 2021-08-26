#pragma once
#include <numeric>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

namespace Performance{
    using seconds = std::chrono::duration<double>;
    using miliseconds = std::chrono::duration<double, std::milli>;
    using microseconds = std::chrono::duration<double, std::micro>;
    using nanoseconds = std::chrono::duration<double, std::nano>;

    enum class TimeUnit{
        s, ms, us, ns, unknown
    };

    std::string get_timeunit_str(TimeUnit unit){
        switch(unit){
            case TimeUnit::s: return "[s]"; break;
            case TimeUnit::ms: return "[ms]"; break;
            case TimeUnit::us: return "[µs]"; break;
            case TimeUnit::ns: return "[ns]"; break;
            default: return "";
        }
    }
    
    template<typename T>
    double get_mean(std::vector<T>& v)
    {
        double sum = std::accumulate(v.begin(), v.end(), 0.0);

        return sum / v.size();
    }


    template<typename T>
    double get_std(std::vector<T>& v)
    {
        double m = get_mean(v);
        double stdDev = std::accumulate(v.begin(), v.end(),  0.0, [m](T a, T b){return a+pow(b-m, 2);});
        stdDev = sqrt(stdDev/v.size());
        return stdDev;

    }

    template<typename T>
    void remove_outlier(std::vector<T>& v)
    {
        double stdDev = get_std(v);
        double m = get_mean(v);
        v.erase(
                std::remove_if(v.begin(), 
                            v.end(),
                            //    [stdDev, m](T x){ return (x<= m-2.5*stdDev)||(x>= m+2.5*stdDev);}), 
                            [stdDev, m](T x){ if( (x<= m-2.5*stdDev)||(x>= m+2.5*stdDev)) 
                                                    {std::cout<<x<<" removed"<<std::endl; return true;} 
                                                return false;}), 
                v.end());
    }
}