#include <bits/stdc++.h>
#include "graph.h"
using namespace std;

// Path dijkstra (const Graph &graph, const Point &from, const Point &to, int fromEdge, int toEdge, bool trace = 0) {
Path dijkstra (const Graph &graph, const CandidatePoint &from, const CandidatePoint &to, bool trace = 0) {
    /// DESC: given a graph, a starting and ending node
    /// return the id of edges lying on the shortest path
    /// while maintain their order

    /// edge-case
    if (from.assocEdge == to.assocEdge) {
        vector<Point> original = graph.edges[from.assocEdge].polyline;
        Path ans;
        for (int i = 0, startTracing = 0; i + 1 < original.size(); i++) {
            if (startTracing) ans.append(original[i]);
            if (!startTracing && pointOnLine(original[i], original[i + 1], from.assocPoint)) {
                ans.append(from.assocPoint);
                startTracing = 1;
            }
            if (startTracing && pointOnLine(original[i], original[i + 1], to.assocPoint)) {
                ans.append(to.assocPoint);
                break;
            }
        }
        ans.addEdge(from.assocEdge);
        return ans;
    }

    int fromNode = graph.edges[from.assocEdge].end;
    int toNode = graph.edges[to.assocEdge].start;

    /// initialize distance, visit-check and trace vector
    vector<double> dist(graph.size() + 1, oo);
    vector<bool> vist(graph.size() + 1);
    vector<int> traceEdge(graph.size() + 1);

    /// initialize priority queue and setup Dijkstra
    using queueItem = pair<double, int>; // <distance, node>
    priority_queue<queueItem> pq;
    pq.emplace(-0.0, fromNode), dist[fromNode] = 0;

    /// run Dijkstra algorithm
    while (pq.size()) {
        int u = pq.top().second; pq.pop();
        if (vist[u]) continue;
        if (u == toNode) break;
        vist[u] = 1;

        // iterate adjacent nodes of u
        for (int edgeID : graph.adj[u]) {
            int v = graph.edges[edgeID].end;
            double weight = graph.edges[edgeID].length;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight, traceEdge[v] = edgeID;
                pq.emplace(-dist[v], v); // I use negative trick for min-heap
            }
        }
    }

    /// prepend and append additional parts of the path
    if (!trace) {
        return Path(dist[toNode] + graph.edges[from.assocEdge].pointToEnd(from.assocPoint).length
             + graph.edges[to.assocEdge].startToPoint(to.assocPoint).length);
    }

    /// add the part of the polyline belonging to the ending-point to the path
    Path path, tmp = graph.edges[to.assocEdge].startToPoint(to.assocPoint);
    tmp.reversePath(), path.extendBack(tmp);
    path.addEdge(to.assocEdge);
    
    /// trace for the middle part of the path
    int curNode = toNode;
    while (curNode != fromNode) {
        int edgeID = traceEdge[curNode];
        vector<Point> pll = graph.edges[edgeID].polyline;
        reverse(pll.begin(), pll.end());
        path.extendBack(pll);
        path.addEdge(edgeID);
        curNode = graph.edges[edgeID].start;
    }

    /// add the part of the polyline belonging to the starting-point to the path
    tmp = graph.edges[from.assocEdge].pointToEnd(from.assocPoint);
    tmp.reversePath(), path.extendBack(tmp);
    path.addEdge(from.assocEdge);

    /// reverse and return the the final path
    path.reversePath();
    return path;
}