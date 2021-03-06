#pragma once
#include <chrono>
#include <string>
#include <iostream>
#include <memory>
#include <source_location>
#include <sstream>
#include "utility.hpp"


class Timer
{
public:
    void start()
    {
        m_StartTime = std::chrono::steady_clock::now();
        m_bRunning = true;
    }
    
    void stop()
    {
        m_EndTime = std::chrono::steady_clock::now();
        m_bRunning = false;
    }

    std::chrono::steady_clock::duration get_duration()
    {
        std::chrono::time_point<std::chrono::steady_clock> endTime;
        
        if(m_bRunning)
        {
            endTime = std::chrono::steady_clock::now();
        }
        else
        {
            endTime = m_EndTime;
        }
        return endTime - m_StartTime;
    }

    double elapsedNanoseconds()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(get_duration()).count();
    }

    double elapsedMicroseconds()
    {
        return elapsedNanoseconds() / 1000.0;
    }

    double elapsedMilliseconds()
    {
        return elapsedMicroseconds() / 1000.0;
    }
    
    double elapsedSeconds()
    {
        return elapsedMilliseconds() / 1000.0;
    }

    
private:
    std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
    std::chrono::time_point<std::chrono::steady_clock> m_EndTime;
    bool                                               m_bRunning = false;
};


class timer_guard{
public:
    timer_guard(std::string name)
    :name(name), dur(nullptr)
    {
        timer.start();
    }
    timer_guard(const std::source_location location = 
               std::source_location::current())
    :dur(nullptr)
    {
        std::stringstream ss;
        ss    <<"\e[1m"
              << location.file_name() << ":"
              << location.line() << ":"
              << location.column() << ":\e[0m";
        line = ss.str();
        timer.start();
    }
    timer_guard(std::chrono::steady_clock::duration* dur, Performance::TimeUnit* unit)
    :dur(dur), unit(unit)
    {
        timer.start();
    }

    ~timer_guard(){
        timer.stop();
        if(dur){
            *dur = timer.get_duration();
            if(timer.elapsedSeconds()>=0.1)
                *unit = Performance::TimeUnit::s;
            else if(timer.elapsedMilliseconds()>=0.1)
                *unit = Performance::TimeUnit::ms;
            else if(timer.elapsedMicroseconds()>=0.1)
                *unit = Performance::TimeUnit::us;
            else
                *unit = Performance::TimeUnit::ns;
        }
        else{
            if(!name.empty())
                std::cout<<name<<" elapsed: ";
            else
                std::cout<<line<<" block elapesd: ";
                
            if(timer.elapsedSeconds()>=0.1)
                std::cout<<timer.elapsedSeconds()<<" [s]"<<"\n";
            else if(timer.elapsedMilliseconds()>=0.1)
                std::cout<<timer.elapsedMilliseconds()<<" [ms]"<<"\n";
            else if(timer.elapsedMicroseconds()>=0.1)
                std::cout<<timer.elapsedMicroseconds()<<" [??s]"<<"\n";
            else
                std::cout<<timer.elapsedNanoseconds()<<" [ns]"<<"\n";
        }
        


    }
private:
    Timer timer;
    std::string name;
    std::string line;
    std::chrono::steady_clock::duration* dur;
    Performance::TimeUnit* unit;
};