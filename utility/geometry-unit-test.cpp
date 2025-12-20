#include <bits/stdc++.h>
#include "geometry.hpp"
using namespace std;

using ll = long long;
using ld = long double;
using pl = pair<ll,ll>;
using pii = pair<int,int>;
using tpl = tuple<int,int,int>;

#define all(a) a.begin(), a.end()
#define filter(a) a.erase(unique(all(a)), a.end())

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    Polyline path({Point(1.5, 0.0), Point(2.0, 1.0), Point(0.0, -1.0), Point(0.5, -0.5)});
    cout << path.length() << "\n";

    cout << path.pop() << " " << path.pop() << " " << path.pop() << "\n";
    cout << path.length() << "\n";

    path.append(Point(3.5, 0.0));
    cout << path.length() << "\n";

    path.append(Point(-10, 0.0));
    cout << path.length() << "\n";

    cout << "size: " << path.size() << "\n";

    return 0;
}