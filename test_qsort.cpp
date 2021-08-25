#include "perf_tool.hpp"
#include "benchmark.hpp"
#include <random>

using namespace std;
namespace pf = Performance;
namespace bm = pf::benchmark;

int main(int argc, char* argv[])
{
    std::default_random_engine eng(1);
    std::uniform_int_distribution<int> dist(0, 1'000'000);
    list<int> l;
    int n = 3'000'000;
    if(argc==2)
        n = atoi(argv[1]);
    {
        timer_guard a("generating numbers");
        while(n--)
        {
            l.push_back(dist(eng));
        }
    }
    
    // {
    //     timer_guard a("parallel_quick_sort");
    //     l = parallel_quick_sort(l);
    // }
    
    pf::measure m([l=l](int cpus){ auto a = l; bm::parallel_quick_sort(a, cpus);}, 1, 50);
    m.print();
    m.to_csv("perf_report/qsort.csv");
    

    // print first 10 elements
    auto a = l.begin();
    auto b = a;
    advance(b, 10);
    for(;a!=b;a++)
    {
        cout<<*a<<endl;
    }

}