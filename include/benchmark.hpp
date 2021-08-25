#pragma once
#include <list>
#include <algorithm>
#include "workstealing_tp.hpp"
#include <thread>

namespace Performance{
    
    namespace benchmark{
        long fibonacci(unsigned n)
        {
            if (n < 2) return n;
            return fibonacci(n-1) + fibonacci(n-2);
        }



        template<class T>
        class sorter{
        public:
            sorter(int threads):pool(threads){}

            ThreadPool pool; 

            std::list<T> do_sort(std::list<T>& chunk_data)
            {
                if(chunk_data.empty())
                {
                    return chunk_data;
                }
                std::list<T> result;
                result.splice(result.begin(),chunk_data,chunk_data.begin());

                T const& partition_val=*result.begin();
                typename std::list<T>::iterator divide_point=
                    std::partition(chunk_data.begin(),chunk_data.end(),
                                [&](T const& val){return val<partition_val;});
                                
                std::list<T> new_lower_chunk;
                new_lower_chunk.splice(new_lower_chunk.end(),
                                    chunk_data,chunk_data.begin(),
                                    divide_point);
                std::future<std::list<T> > new_lower=           
                    pool.submit(std::bind(&sorter::do_sort,this,
                                        std::move(new_lower_chunk)));


                std::list<T> new_higher(do_sort(chunk_data));
                result.splice(result.end(),new_higher);
                while(new_lower.wait_for(std::chrono::seconds(0)) ==
                    std::future_status::timeout)
                {
                    pool.run_pending_task();                    
                }
                result.splice(result.begin(),new_lower.get());
                return result;
            }
        };


        template<typename T>
        std::list<T> parallel_quick_sort(std::list<T>& input, int cpus = std::thread::hardware_concurrency())
        {
            if(input.empty())
            {
                return input;
            }
            sorter<T> s(cpus);
            return s.do_sort(input);
        }
    }
}