#include <bits/stdc++.h>
#include "../utility/graph.hpp"
using namespace std;

const int DIJKSTRA_THRESHOLD = 75;
const int DIJKSTRA_NODE = 2e5 + 5;
const int shifts[3] = {0, 8, 16};

char cache[6 * DIJKSTRA_THRESHOLD];
int trace[DIJKSTRA_NODE], cursor = 0;

int readNextInt() {
    int value = 0;
    for (int i = 0; i < 3; i++, cursor++)
        value |= (cache[cursor] << shifts[i]);
    return value;
}

// SIMPLE DIJKSTRA QUERY THAT RETURNS THE EDGE ID OF
// THE SHORTEST PATH FROM `FROM` TO `TO`
vector<int> dijkstra (const Graph &G, int from, int to, string UBODT) {
    // Read the appropriate data from UBODT
    ifstream fin(UBODT, ios::binary);
    fin.seekg(6 * DIJKSTRA_THRESHOLD * (from - 1), ios::beg);
    fin.read(cache, 6 * DIJKSTRA_THRESHOLD);
    fin.close();

    // Extract data
    vector<int> nodeList(DIJKSTRA_THRESHOLD);
    cursor = 0;
    for (int &u : nodeList)
        u = readNextInt(), trace[u] = readNextInt();
    
    // Check if to is found in the list
    bool found = 0;
    for (int u : nodeList) {
        if (u == to) {
            found = 1;
            break;
        }
        if (!u) return vector<int>(0); // `from` is isolated, return empty list
    }

    // Run Dijkstra from scratch
    if (!found) {
        vector<double> dist(G.size() + 1, oo);
        vector<bool> vist(G.size() + 1, false);

        using item = pair<double,int>;
        priority_queue<item, vector<item>, greater<item>> pq;
        pq.emplace(0, from), dist[from] = 0;

        while (pq.size()) {
            int u = pq.top().second; pq.pop();
            if (vist[u]) continue;
            vist[u] = true;
            if (u == to) break;

            for (int edgeID : G.adj[u]) {
                int v = G.edges[edgeID].to;
                double weight = G.edges[edgeID].length();
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight, trace[v] = edgeID;
                    pq.emplace(dist[v], v);
                }
            }
        }
    }

    // Trace for edge IDs of the shortest path
    vector<int> edgeID;
    while (from != to) {
        int curEdge = trace[to];
        edgeID.push_back(curEdge), to = G.edges[curEdge].from;
    }
    reverse(edgeID.begin(), edgeID.end());
    return edgeID;
}

Polyline shortestPath (const Graph &G, const CandidatePoint &from, const CandidatePoint &to);