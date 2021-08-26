#pragma once
#include "timer.hpp"
#include "range.hpp"
#include "utility.hpp"
#include <thread>
#include <optional>
#include <sstream>
#include <functional>
#include <fstream>

namespace Performance{
    struct DistData{
        int freq;
        std::optional<double> lowerbound, upperbound; // [,)

        std::string get_str(){
            std::ostringstream oss;
            if(lowerbound.has_value() && upperbound.has_value())
            {
                oss<<*lowerbound<<"~"<<*upperbound<<","<<freq<<std::endl;
                return oss.str();
            }
            else if(lowerbound.has_value())
            {
                oss<<">="<<*lowerbound<<","<<freq<<std::endl;
                return oss.str();
            }
            else if(upperbound.has_value())
            {
                oss<<"<"<<*upperbound<<","<<freq<<std::endl;
                return oss.str();
            }
            else{
                throw std::runtime_error("DistData error");
            }
        }
    };
    

    class TestData{
    public:
        TestData(int thread, int problem_size, int repeat)
        :thread(thread), problem_size(problem_size), repeat(repeat), durs(repeat), unit(TimeUnit::unknown) 
        { }

        void analyze(){
            remove_outlier(times);
            mean = get_mean(times);
            std = get_std(times);
            compute_dist();
        }

        void compute_dist(){
            int nInterval = 10;
            dist.resize(nInterval);
            double interval = std / 2.0;
            double upper = mean - (nInterval-1)/2* interval;
            double lower;
            dist[0].freq = count_if(times.begin(), times.end(), [upper](auto& t){return t<upper;});
            dist[0].upperbound = upper;

            for(int j=1; j<nInterval-1; j++)
            {
                lower = upper;
                upper += interval;
                dist[j].freq = count_if(times.begin(), times.end(), [lower, upper](auto& t){return lower<=t && t<upper;});
                dist[j].lowerbound = lower;
                dist[j].upperbound = upper;
            }
            lower = upper;
            dist[nInterval-1].freq = count_if(times.begin(), times.end(), [lower](auto& t){return lower<=t;});
            dist[nInterval-1].lowerbound = lower;
        }

        

        void output_analysis(std::ostream& os)
        {
            // thread, problem_size, mean, std, speedup
            os<<thread<<","<<problem_size<<","<<mean<<","<<std<<","<<baseline->mean/mean<<std::endl;
            
        }

        void output_dist(std::ostream& os)
        {
            for(auto& data: dist)
            {
                os<<data.get_str();
            }
        }

        void output_rawdata(std::ostream& os)
        {
            os<<"thread,serial,time ";
            switch(unit){
                case TimeUnit::s: os<<"[s]"; break;
                case TimeUnit::ms: os<<"[ms]"; break;
                case TimeUnit::us: os<<"[µs]"; break;
                case TimeUnit::ns: os<<"[ns]"; break;
            }
            os<<std::endl;
            
            int serial = 0;
            for(auto& t: times)
            {
                os<<thread<<","<<serial++<<","<<t<<std::endl;
            }
        }

        /* print:
            1. analysis
            2. dist        */
        void print()
        {
            std::cout<<"thread,problem size,mean "<<get_timeunit_str(unit)<<",std,speedup"<<std::endl;
            output_analysis(std::cout);
            std::cout<<std::endl;
            
            std::cout<<"thread: "<<thread<<","<<"problem size: "<<problem_size<<std::endl;
            std::cout<<"range,freq"<<std::endl;
            output_dist(std::cout);
        }

        // print: raw_data
        void print_rawdata()
        {
            output_rawdata(std::cout);
        }


        int thread;
        int problem_size;
        int repeat;
        double std;
        double mean;
        std::vector<std::chrono::steady_clock::duration> durs;
        std::vector<double> times;
        std::vector<DistData> dist;
        TimeUnit unit;
        TestData* baseline;
    };

    class Test{
    public:
        Test(   std::function<void(int)> task, std::function<void(int)> data_generator, 
                       std::vector<int> t_range = range(1,2), 
                       std::vector<int> size_range = range(1), int repeat = 10)
        :task(task), data_generator(data_generator), data(t_range.size())
        {
            // warmup
            for(int i=0; i<5; ++i)
            {
                data_generator(size_range[0]);
                task(t_range.back());
            }

            for (int i = 0; i < t_range.size(); i++) {
                for (int j = 0; j < size_range.size(); j++) {
                    data[i].emplace_back(t_range[i], size_range[j], repeat);
                    data[i][j].baseline = &data[0][j];
                    auto& durs = data[i][j].durs;
                    auto& times = data[i][j].times;
                    auto& baseline = data[i][j].baseline;

                    TimeUnit unit;
                    for (int k = 0; k < repeat; k++)
                    {
                        data_generator(size_range[j]);
                        {
                            timer_guard tg(&durs[k], &unit);
                            task(t_range[i]);
                        }
                        if(baseline->unit == TimeUnit::unknown)
                            baseline->unit = unit;
                    }

                    unit = baseline->unit;
                    std::for_each(durs.begin(), durs.end(),  
                        [&times, unit](auto& dur){
                            switch(unit){
                                case TimeUnit::s:  times.push_back(seconds(dur).count()); break;
                                case TimeUnit::ms: times.push_back(miliseconds(dur).count()); break;
                                case TimeUnit::us: times.push_back(microseconds(dur).count()); break;
                                case TimeUnit::ns: times.push_back(nanoseconds(dur).count()); break;
                            }
                        }
                    );
                    data[i][j].analyze();
                }
            }
        }

        void output_analysis(std::ostream& os)
        {
            // thread, problem_size, mean, std, speedup
            for (int i = 0; i < data[0].size(); i++) {
                os<<"thread,problem_size,mean,std,speedup"<<std::endl;
                for (int j = 0; j < data.size(); j++) {
                    data[j][i].output_analysis(os);
                }
                os<<std::endl;
            }
        }

        void output_dist(std::ostream& os)
        {
            for (int i = 0; i < data[0].size(); i++) {
                for (int j = 0; j < data.size(); j++) {
                    os<<"thread,problem size,mean,std,speedup"<<std::endl;
                    data[j][i].output_dist(os);
                    os<<std::endl;
                }
                os<<std::endl;

            }
        }

        void output_rawdata(std::ostream& os)
        {
            for (int i = 0; i < data[0].size(); i++) {
                for (int j = 0; j < data.size(); j++) {
                    data[j][i].output_rawdata(os);
                    os<<std::endl;
                }
                os<<std::endl;

            }
        }

        void print(){
            for (int i = 0; i < data[0].size(); i++) {
                for (int j = 0; j < data.size(); j++) {
                    data[j][i].print();
                }
                std::cout<<std::endl;
            }
        }

        void to_csv(std::string name){
            std::string tail(".csv");
            std::ofstream fout(name.append(tail));
            output_analysis(fout);
            output_dist(fout);

            tail = "_raw.csv";
            std::ofstream raw(name.append(tail));
            output_rawdata(raw);
        }
            
        
        std::function<void(int)> task;
        std::function<void(int)> data_generator;
        // data[thread][problem_size]
        std::vector<std::vector<TestData>> data;
    };

}