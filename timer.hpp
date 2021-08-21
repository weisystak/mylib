#include <chrono>
#include <string>
#include <iostream>

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
    
    double elapsedMicorseconds()
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
        
        return std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_StartTime).count();
    }

    double elapsedMilliseconds()
    {
        return elapsedMicorseconds() / 1000.0;
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
    timer_guard(std::string name="unknown"):name(name){
        timer.start();
    }
    timer_guard(int line):line(line){
        timer.start();
    }
    ~timer_guard(){
        timer.stop();
        if(!name.empty())
            std::cout<<name<<" elapsed: ";
        else
            std::cout<<"line "<<line<<" block elapesd: ";
        if(timer.elapsedSeconds()>=0.1)
            std::cout<<timer.elapsedSeconds()<<" [s]"<<"\n";
        else if(timer.elapsedMilliseconds()>=0.1)
            std::cout<<timer.elapsedMilliseconds()<<" [ms]"<<"\n";
        else
            std::cout<<timer.elapsedMicorseconds()<<" [µs]"<<"\n";

    }
private:
    Timer timer;
    std::string name;
    int line;
};