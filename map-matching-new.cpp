#include <bits/stdc++.h>
#include "utility/helper-algorithms.hpp"
using namespace std;

using ll = long long;
using ld = long double;
using pl = pair<ll,ll>;
using pii = pair<int,int>;
using tpl = tuple<int,int,int>;

#define all(a) a.begin(), a.end()
#define filter(a) a.erase(unique(all(a)), a.end())

const int MAXCANDIDATE = 8;

namespace timing {
    auto start = chrono::high_resolution_clock::now();
    auto currentFlag = chrono::high_resolution_clock::now();

    double curTime() { // may lead to an approximately 0.01s offset
        auto cur = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = cur - currentFlag;
        return elapsed.count();
    }

    double totalTime() {
        auto cur = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = cur - start;
        return elapsed.count();
    }

    void flag() { currentFlag = chrono::high_resolution_clock::now(); }
};

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    /// Read input
    cout << "Reading input graph..." << endl, timing::flag();
    Graph G = readGraph("evaluation/network.bin", "bin");
    cout << "Done in " << timing::curTime() << " seconds" << endl;
    cout << "Receive graph with " << G.size() << " nodes and " << G.edgeCount() << " edges\n";
    
    cout << "Reading raw trajectory..." << endl, timing::flag();
    vector<CandidatePoint> trajectory = readTrajectory("evaluation/raw-path.txt", "txt");
    cout << "Done in " << timing::curTime() << " seconds" << endl;
    
    /// Find the candidate list
    cout << "Computing candidate list..." << endl, timing::flag();
    vector<vector<CandidatePoint>> candList = findCandidates(G, trajectory, MAXCANDIDATE);
    cout << "Done in " << timing::curTime() << " seconds" << endl;
    
    /// Find the optimal path on the virtual graph
    cout << "Finding optimal path..." << endl, timing::flag();
    vector<int> optimalPath = bestScoringPath(G, candList);
    cout << "Done in " << timing::curTime() << " seconds" << endl;

    /// Find the geometry of the optimal path
    cout << "Finding the geometry of the optimal path..." << endl, timing::flag();
    Polyline ans;
    for (int i = 0; i + 1 < optimalPath.size(); i++) {
        CandidatePoint prv = candList[i][optimalPath[i]], nxt = candList[i + 1][optimalPath[i + 1]];
        ans += shortestPath(G, prv, nxt);        
    }
    cout << "Done in " << timing::curTime() << " seconds" << endl;

    /// Output the geometry of the optimal path
    cout << "Writing the geometry of the optimal path..." << endl, timing::flag();
    writeMatchedTrajectory("evaluation/matched-path.txt", "txt", ans);
    cout << "Done in " << timing::curTime() << " seconds" << endl;

    cout << "Program done in " << timing::totalTime() << " seconds" << endl;

    return 0;
}