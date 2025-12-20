#include <bits/stdc++.h>
#include "utility/dynamic-programming.h"
using namespace std;

using ll = long long;
using ld = long double;
using pl = pair<ll,ll>;
using pii = pair<int,int>;
using tpl = tuple<int,int,int>;

#define all(a) a.begin(), a.end()
#define filter(a) a.erase(unique(all(a)), a.end())

const double MAXRADIUS = 1000; // constants are choosen according to the paper
const int MAXCANDIDATE = 4; // constants are choosen according to the paper (multiplied by 2 since I use virtual directed edges)
int N, M, K;

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    cerr << "Started running..." << endl;

    /// read input and prepare graph
    ifstream inp("build/network.txt");
    inp >> N >> M;
    vector<Edge> edges(M);

    for (int i = 0; i < M; i++) {
        int u, v, isOneway; double limit; inp >> u >> v >> limit >> isOneway;
        int polySize; inp >> polySize;
        vector<Point> polyline(polySize);
        for (Point &p : polyline) inp >> p;
        edges[i] = Edge(u, v, limit, polyline, isOneway);
    }
    Graph graph(N, edges);
    inp.close();

    inp = ifstream("build/trajectory.txt");
    inp >> K;
    vector<Point> trajectory(K);
    vector<double> recTime(K);
    for (int i = 0; i < K; i++)
        inp >> trajectory[i] >> recTime[i];
    inp.close();

    /// prepare candidate list for each point of the trajectory
    vector<vector<CandidatePoint>> candidates(K);
    for (int i = 0; i < K; i++) {
        vector<pair<double,int>> distances(graph.edgeCount());
        for (int eID = 0; eID < graph.edgeCount(); eID++)
            distances[eID] = make_pair(graph.edges[eID].euclideDistance(trajectory[i]), eID);
        sort(distances.begin(), distances.end());

        // cerr << "shortest distance " << distances[0].first << endl;

        vector<CandidatePoint> vec;
        for (int j = 0; j < graph.edgeCount() && j < MAXCANDIDATE; j++) {
            int edgeID = distances[j].second;
            vec.push_back(CandidatePoint(trajectory[i], edgeID, graph.edges[edgeID], recTime[i]));
        }
        candidates[i] = vec;
        assert(vec.size());

        // cerr << "for layer " << i << ", found " << vec.size() << " candidates" << endl;
    }

    /// find the optimal path on virtual DAG
    vector<int> optimal = dynamicProgramming(graph, candidates);

    /// trace for original path on the map
    Path optimalPath;
    for (int layer = 0; layer + 1 < K; layer++) {
        CandidatePoint p1 = candidates[layer][optimal[layer]], p2 = candidates[layer + 1][optimal[layer + 1]];
        Path cur = dijkstra(graph, p1, p2, 1);

        // cerr << trajectory[layer] << " --> " << trajectory[layer + 1] << " ";
        // cerr << p1.assocEdge << " " << p2.assocEdge << " " << spatialAnalysis(graph, p1, p2) << " " << temporalAnalysis(graph, p1, p2) << "\n";
        // cerr << SPFunction(graph, candidates[layer][optimal[layer]], candidates[layer + 1][optimal[layer + 1]]) << "\n";
        // cerr << "Immediate path: " << cur << "\n";
        optimalPath.extendBack(cur);
    }

    // cerr << optimalPath << "\n";

    ofstream ouf("build/output.txt");
    ouf << optimalPath.size() << "\n";
    ouf << fixed << setprecision(9);
    for (Point u : optimalPath.polyline) ouf << u.x << " " << u.y << "\n";
    ouf.close();

    return 0;
}