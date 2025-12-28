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

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    /// Read input
    cout << "Reading input graph..." << endl;
    Graph G = readGraph("evaluation/helper-files/network.txt", "txt");
    cout << "Done!" << endl;

    cout << "Reading raw trajectory..." << endl;
    vector<CandidatePoint> trajectory = readTrajectory("evaluation/helper-files/raw-path.txt", "txt");
    cout << "Done!" << endl;

    /// Find the candidate list
    cout << "Computing candidate list..." << endl;
    vector<vector<CandidatePoint>> candList = findCandidates(G, trajectory, MAXCANDIDATE);
    cout << "Done!" << endl;

    // cout << "\n";
    // for (int i = 0; i < candList.size(); i++) {
    //     cout << "Layer " << i << ": ";
    //     for (CandidatePoint pt : candList[i]) cout << "(" << pt.gps << "/" << pt.assocEdge << ") ";
    //     cout << "\n";
    // }
    // cout << "\n";

    /// Find the optimal path on the virtual graph
    cout << "Finding optimal path..." << endl;
    vector<int> optimalPath = bestScoringPath(G, candList);
    cout << "Done!" << endl;

    // cout << "Optimal path: ";
    // for (int u : optimalPath) cout << u << " ";
    // cout << "\n";

    /// Find the geometry of the optimal path
    cout << "Finding the geometry of the optimal path..." << endl;
    Polyline ans;
    for (int i = 0; i + 1 < optimalPath.size(); i++) {
        CandidatePoint prv = candList[i][optimalPath[i]], nxt = candList[i + 1][optimalPath[i + 1]];
        ans += shortestPath(G, prv, nxt);
        // vector<int> edgeIDs = dijkstra(G, G.edges[prv.assocEdge].to, G.edges[nxt.assocEdge].from);
        // cout << "Path " << i + 1 << ": ";
        // for (int u : edgeIDs) cout << u << " ";
        // cout << "\n";
        
    }
    cout << "Done!" << endl;

    /// Output the geometry of the optimal path
    cout << "Writing the geometry of the optimal path..." << endl;
    writeMatchedTrajectory("evaluation/helper-files/matched-path.txt", "txt", ans);
    cout << "Done!" << endl;

    return 0;
}