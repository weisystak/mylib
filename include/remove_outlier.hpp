#pragma once
#include <numeric>
#include <cmath>
#include <vector>
#include <iostream>

template<typename T>
double mean(std::vector<T>& v)
{
    double sum = std::accumulate(v.begin(), v.end(), 0.0);

    return sum / v.size();
}


template<typename T>
double stddev(std::vector<T>& v)
{
    double m = mean(v);
    double stdDev = std::accumulate(v.begin(), v.end(),  0.0, [m](T a, T b){return a+pow(b-m, 2);});
    stdDev = sqrt(stdDev/v.size());
    return stdDev;

}

template<typename T>
void remove_outlier(std::vector<T>& v)
{
    double stdDev = stddev(v);
    double m = mean(v);
    v.erase(
            std::remove_if(v.begin(), 
                           v.end(),
                        //    [stdDev, m](T x){ return (x<= m-2.5*stdDev)||(x>= m+2.5*stdDev);}), 
                           [stdDev, m](T x){ if( (x<= m-2.5*stdDev)||(x>= m+2.5*stdDev)) 
                                                {std::cout<<x<<" removed"<<std::endl; return true;} 
                                              return false;}), 
            v.end());
}
