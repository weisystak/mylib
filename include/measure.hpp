#pragma once
#include "test.cpp"
#include <thread>

namespace Performance{
    class Measure{
        measure m();

        void push_test(Test t){
            tests.push_back(test);
        }

        
        std::vector<Test> tests;

    };
}
