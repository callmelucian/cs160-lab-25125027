#include <bits/stdc++.h>
#include "utility/graph.h"

Path readPath (const string &filePath) {
    ifstream inf(filePath);
    int K; inf >> K;

    vector<Point> vec(K);
    for (Point &it : vec) inf >> it;
    inf.close();
    return Path(vec, {});
}

int main()
{
    Path rawPath = readPath("build/output.txt");
    Path truePath = readPath("build/answer.txt");

    ofstream ouf("build/verdict.txt");
    // ouf << fixed << setprecision(9) << overlapRatio(rawPath, truePath) << "\n";
    cerr << fixed << setprecision(9) << overlapRatio(rawPath, truePath) << "\n";
    ouf.close();

    return 0;
}