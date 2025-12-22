#include <bits/stdc++.h>
#include "../utility/graph.hpp"
using namespace std;

using ll = long long;
using ld = long double;
using pl = pair<ll,ll>;
using pii = pair<int,int>;
using tpl = tuple<int,int,int>;

#define all(a) a.begin(), a.end()
#define filter(a) a.erase(unique(all(a)), a.end())

const int mn = 2e5 + 5;

const double oo = 1e9;
const int mn = 2e5 + 5;
const int thres = 75;
double dist[mn]; // store the distance of each node
bool vist[mn]; // check if a node is visited
int trace[mn]; // as explain in documentation

vector<int> runDijkstra (int source, int N, const Graph& G) {
    using item = pair<double,int>;
    priority_queue<item, greater<item>, vector<item>> pq;
    vector<int> nodeList;

    pq.emplace(0, source), dist[source] = 0;
    while (pq.size() && nodeList.size() < thres) {
        int u = pq.top().second; pq.pop();
        if (vist[u]) continue;
        if (u != source) nodeList.push_back(u);

        for (int edgeID : G.adj[u]) {
            int v = G.edges[edgeID].to;
            double weight = G.edges[edgeID].length();
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight, trace[v] = u;
                pq.emplace(dist[v], v);
            }
        }
    }
    return nodeList;
}

const int mask = 0xFF;
const int shifts[3] = {0, 8, 16};

void prepareBinaryFile (const vector<int> &nodeList, string fileName) {
    ofstream fout(fileName, ios::binary | ios::app);
    if (fout.is_open()) {
        function<void(int)> writeThreeBytes = [&] (int value) {
            char bytes[3];
            for (int i = 0; i < 3; i++)
                bytes[i] = (value >> shifts[i]) & mask;
            fout.write(bytes, 3);
        };

        if (nodeList.size() != thres) {
            cout << "Size of nodeList is not consistent!\n";
            exit(0);
        }
        for (int u : nodeList)
            writeThreeBytes(u), writeThreeBytes(trace[u]);
    }
    fout.close();
}

void resetNode (int u) { dist[u] = oo, vist[u] = trace[u] = 0; }

void reinitialize (const vector<int> &nodeList, int source) {
    for (int u : nodeList) resetNode(u);
    resetNode(source);
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    

    return 0;
}