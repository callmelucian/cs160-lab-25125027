#include <bits/stdc++.h>
#include "../utility/dijkstra.hpp"
using namespace std;

using ll = long long;
using ld = long double;
using pl = pair<ll,ll>;
using pii = pair<int,int>;
using tpl = tuple<int,int,int>;

#define all(a) a.begin(), a.end()
#define filter(a) a.erase(unique(all(a)), a.end())

double trueDijkstra (const Graph& G, const CandidatePoint &from, const CandidatePoint &to) {
    vector<double> dist(G.size() + 1, oo);
    vector<bool> vist(G.size() + 1, false);

    int fromNode = G.edges[from.assocEdge].to;
    int toNode = G.edges[to.assocEdge].from;
    double add = G.edges[from.assocEdge].polyline.extract(from.gps, 1).length()
               + G.edges[to.assocEdge].polyline.extract(to.gps, 0).length();

    using item = pair<double,int>;
    priority_queue<item, vector<item>, greater<item>> pq;
    pq.emplace(0, fromNode), dist[fromNode] = 0;

    while (pq.size()) {
        int u = pq.top().second; pq.pop();
        if (vist[u]) continue;
        vist[u] = 1;
        if (u == toNode) return dist[u] + add;

        for (int edgeID : G.adj[u]) {
            int v = G.edges[edgeID].to;
            double w = G.edges[edgeID].length();
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return oo;
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    Graph G = readGraph("../evaluation/network.txt", "txt");
    cout << "Found " << G.edgeCount() << " edges, " << G.size() << " nodes" << endl;

    ifstream fin("../evaluation/raw-path.txt");
    cout << "Start reading raw path..." << endl;
    int k; fin >> k;
    vector<CandidatePoint> rawPath(k);
    int counter = 0;
    for (CandidatePoint &pt : rawPath) {
        fin >> pt.gps >> pt.recordTime;
        vector<double> dists(G.edgeCount());
        for (int i = 0; i < dists.size(); i++)
            dists[i] = euclideDist(G.edges[i].polyline, pt.gps);
        pt.assocEdge = min_element(dists.begin(), dists.end()) - dists.begin();
    }
    cout << "Finished!" << endl;
    fin.close();

    cout << "Found " << rawPath.size() << " raw points!" << endl;

    cout << fixed << setprecision(9);
    for (int i = 0; i + 1 < rawPath.size(); i++) {
        double trueValue = trueDijkstra(G, rawPath[i], rawPath[i + 1]);
        double compValue = shortestPathLength(G, rawPath[i], rawPath[i + 1]);
        if (abs(trueValue - compValue) < eps)
            cout << "Okay " << trueValue << " " << compValue << endl;
        else {
            cout << "Wrong answer " << i << "\n";
            cout << trueValue << " " << compValue << "\n";
        }
    }

    return 0;
}