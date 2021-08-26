#include "test.hpp"
#include "benchmark.hpp"

using namespace std;
namespace pf = Performance;
namespace bm = pf::benchmark;

int main()
{
    auto task = [f=bm::fibonacci](int n){ (f)(30); };
    pf::Test t(task, [](int n){}, pf::range(1), pf::range(1), 10);
    t.print();
    t.to_csv("f");
}