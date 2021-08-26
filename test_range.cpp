#include "range.hpp"
#include <iostream>

using namespace std;
using namespace Performance;

int main(int argc, char* argv[])
{
    for(auto& i : range(10, 20, 2))
        cout<<i<<endl;
    cout<<endl;
    for(auto& i : range(10))
        cout<<i<<endl;
    cout<<endl;
    for(auto& i : range(1, 10))
        cout<<i<<endl;
}