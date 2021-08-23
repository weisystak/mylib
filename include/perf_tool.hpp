#pragma once
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>
#include "timer.hpp"
#include "remove_outlier.hpp"
#include <source_location>

namespace Performance{
    // utility
    template<typename T> //  TD declared only for printing deduced type
    class TD; 

    class measure{
    public:
        using microseconds = std::chrono::duration<double, std::micro>;

        measure(std::function<void(int)>&& f, int threads=std::thread::hardware_concurrency(), int repeat=5)
        :f(f),threads(threads), repeat(repeat), v(threads, std::vector<std::chrono::steady_clock::duration>(repeat))
        {
            // warmup
            for(int i=0; i<5; ++i)
                f(threads);

            for(int i = 0; i < threads; i++)
            {
                for(int j = 0; j < repeat; j++)
                {
                    timer_guard tg(&v[i][j]);
                    (f)(i+1);
                }
                std::vector<double> tmp;
                std::for_each(v[i].begin(), v[i].end(),  
                              [&tmp](auto& dur){ 
                                  tmp.push_back(microseconds(dur).count());
                              });
                us.push_back(std::move(tmp));
                remove_outlier(us[i]);
            }
            baseline_time = mean(us[0]);

        }

        void print(){
            for(int i = 0; i<us.size(); i++)
            {
                double m = mean(us[i]);
                double stdDev = stddev(us[i]);
                std::cout<<"f("<<i+1<<")"<<" consuming time [µs]"<<std::endl;
                std::cout<<"mean: "<<m<<std::endl;
                std::cout<<"std: "<<stdDev<<std::endl;
                std::cout<<"speedup: "<<baseline_time/m<<std::endl<<std::endl;

            }
        }

        void to_csv(){
            // FIXME
            for(int i = 0; i<us.size(); i++)
            {
                double m = mean(us[i]);
                double stdDev = stddev(us[i]);
                std::cout<<"f("<<i+1<<")"<<" consuming time [µs]"<<std::endl;
                std::cout<<"mean: "<<m<<std::endl;
                std::cout<<"std: "<<stdDev<<std::endl;
                std::cout<<"speedup: "<<baseline_time/m<<std::endl<<std::endl;

            }
        }
    private:
        std::function<void(int)> f;
        int threads;
        int repeat;
        std::vector<std::vector<std::chrono::steady_clock::duration>> v;
        std::vector<std::vector<double>> us;
        double baseline_time;
    };

    
}