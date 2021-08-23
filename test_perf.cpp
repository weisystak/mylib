#include "perf_tool.hpp"
#include "benchmark.hpp"

using namespace std;
namespace pf = Performance;
namespace bm = pf::benchmark;

int main()
{
    // auto f =[f=&bm::fibonacci](int n){ (*f)(n); };
    // pf::TD<decltype(f)> ftype;
    auto func =  [f=bm::fibonacci](int n){ (f)(n); };
    pf::measure m(func);
    m.print();
}