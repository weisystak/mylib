#include "remove_outlier.hpp"
#include <bits/stdc++.h>

using namespace std;

int main()
{
    vector<int> v{1,3,434,34,67,5,65,57,6,54,57,454,26,57,8};
    double m = mean(v);
    double d = stddev(v);
    cout<<"mean: "<<m<<"\n"<<"std: "<<d<<"\n";
    // 454 is removed
    remove_outlier(v);
    for(auto& x : v)
        cout<<x<<" ";
}