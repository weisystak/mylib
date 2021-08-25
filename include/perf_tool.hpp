#pragma once
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>
#include "timer.hpp"
#include "remove_outlier.hpp"
#include <source_location>
#include <fstream>

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

        void freq_to_cst(std::ofstream& fout, std::vector<double>& v, int i)
        {
            fout<<"thread,range,freq"<<std::endl;
            double m = mean(v);
            double stdDev = stddev(v);
            double interval = stdDev * 2.0 * 2.0 / 10.0;
            m = m - 5.0 * interval;
            double prev = 0.0;
            fout<<i<<",<"<<m<<","<<count_if(v.begin(), v.end(), [m](auto& t){return t<m;})<< std::endl;
            for(int j=0; j<10; j++)
            {
                prev = m;
                m += interval;
                fout<<i<<","<<prev<<"~"<<m<<","<<count_if(v.begin(), v.end(), [prev, m](auto& t){return prev<=t && t<m;})<< std::endl;
            }
            fout<<i<<",>="<<m<<","<<count_if(v.begin(), v.end(), [m](auto& t){return m<=t;})<< std::endl <<std::endl;

        }
        template<typename T>
        void to_csv(T&& name){
            // raw data
            std::ofstream fout(std::forward<T>(name));
            fout<<"thread,serial,time [µs]"<<std::endl;
            
            for(int i = 0; i<us.size(); i++)
            {
                int n = 0;
                for(auto& t:us[i])
                {
                    fout<<i+1<<","<<n++<<","<<t<<std::endl;
                }
            }

            fout<<std::endl;
            fout<<"thread,mean,std,speedup"<<std::endl;

            // mean, std, speedup
            double m, stdDev, speedup;
            for(int i = 0; i<us.size(); i++)
            {
                double m = mean(us[i]);
                double stdDev = stddev(us[i]);
                fout<<i+1<<","<<m<<","<<stdDev<<","<<baseline_time/m<<std::endl;
            }
            fout<<std::endl;

            // histogram
            for(int i = 0; i<us.size(); i++)
            {
                freq_to_cst(fout, us[i], i+1);
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