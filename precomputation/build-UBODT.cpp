#include <bits/stdc++.h>
#include "../utility/graph.hpp"
#include "utility.hpp"
using namespace std;

using ll = long long;
using ld = long double;
using pl = pair<ll,ll>;
using pii = pair<int,int>;
using tpl = tuple<int,int,int>;

#define all(a) a.begin(), a.end()
#define filter(a) a.erase(unique(all(a)), a.end())

const int mn = 2e5 + 5;
const int thres = 75;
double dist[mn]; // store the distance of each node
bool vist[mn]; // check if a node is visited
int trace[mn]; // as explain in documentation
vector<int> toReset;
int countIsolated;

vector<int> runDijkstra (int source, const Graph& G) {
    using item = pair<double,int>;
    priority_queue<item, vector<item>, greater<item>> pq;
    vector<int> nodeList;

    pq.emplace(0, source), dist[source] = 0;
    while (pq.size() && nodeList.size() < thres) {
        int u = pq.top().second; pq.pop();
        if (vist[u]) continue;
        if (u != source) nodeList.push_back(u);
        vist[u] = true, toReset.push_back(u);

        for (int edgeID : G.adj[u]) {
            int v = G.edges[edgeID].to;
            double weight = G.edges[edgeID].length();
            if (dist[u] + weight < dist[v]) {
                toReset.push_back(v);
                dist[v] = dist[u] + weight, trace[v] = edgeID;
                pq.emplace(dist[v], v);
            }
        }
    }
    if (nodeList.size() < thres)
        cout << "Node " << source << " is isolated" << endl, countIsolated++;
    while (nodeList.size() < thres) nodeList.push_back(0);
    return nodeList;
}

void prepareBinaryFile (const vector<int> &nodeList, string fileName, int source) {
    ofstream fout(fileName, ios::binary | ios::app);
    if (fout.is_open()) {
        if (nodeList.size() != thres) {
            cout << "Size of nodeList is not consistent!\n";
            cout << "Found " << nodeList.size() << " on current source " << source << "\n";
            exit(0);
        }
        for (int u : nodeList)
            writeThreeBytes(fout, u), writeThreeBytes(fout, trace[u]);
    }
    fout.close();
}

void resetNode (int u) {
    dist[u] = oo, trace[u] = 0, vist[u] = false;
}

Graph readGraph (string fileName, string mode) {
    if (mode == "txt") {
        cout << "Start reading TXT file..." << endl;
        ifstream fin(fileName);
        int N, M; fin >> N >> M;
        vector<Edge> edges(M);
        vector<bool> isOneway(M);

        for (int i = 0; i < M; i++) {
            int u, v, oneway; double speedLimit;
            fin >> u >> v >> speedLimit >> oneway;

            int polylength; fin >> polylength;
            vector<Point> pll(polylength);
            for (Point &it : pll) fin >> it;

            edges[i] = Edge(u, v, speedLimit, pll);
            isOneway[i] = oneway;
        }
        cout << "Building graph..." << endl;
        return Graph(N, edges, isOneway);
    }
    else return Graph();
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    Graph G = readGraph("../build/network.txt", "txt");
    cout << "Received graph with " << G.size() << " nodes and " << G.edgeCount() << " edges" << endl;

    cout << "Running Dijkstra..." << endl;
    for (int i = 1; i <= G.size(); i++) resetNode(i);
    
    for (int source = 1; source <= G.size(); source++) {
        if (source % 2000 == 0) {
            cout << "Checkpoint: " << source << "-th node..." << endl;
        }
        vector<int> nodeList = runDijkstra(source, G);
        prepareBinaryFile(nodeList, "UBODT.bin", source);

        for (int u : toReset) resetNode(u);
        resetNode(source), toReset.clear();
    }

    cout << "Done with " << countIsolated << " isolated nodes" << endl;


    return 0;
}