#include <bits/stdc++.h>
#include "../utility/geometry.hpp"
using namespace std;

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    cout << "FIRST UNIT TEST:\n";
    Polyline path({Point(1.5, 0.0), Point(2.0, 1.0), Point(0.0, -1.0), Point(0.5, -0.5)});
    cout << path.length() << "\n";

    cout << path.pop() << " " << path.pop() << " " << path.pop() << "\n";
    cout << path.length() << "\n";

    path.append(Point(3.5, 0.0));
    cout << path.length() << "\n";

    path.append(Point(-10, 0.0));
    cout << path.length() << " " << path.size() << "\n\n";

    cout << "SECOND UNIT TEST:\n";
    path.clear();
    path = Polyline({Point(0, 0), Point(8, 3), Point(2, 4), Point(6, 7), Point(11, 6), Point(10, 9), Point(7, 10)});
    cout << path.extract(Point(4, 3), 1) << "\n";
    cout << path.extract(Point(4, 3), 0) << "\n";

    return 0;
}