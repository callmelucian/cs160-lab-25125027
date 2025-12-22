#include <bits/stdc++.h>
#include "geometry.hpp"
using namespace std;

// A DIRECTED EDGE STRUCT
struct Edge {
    int from, to;
    double speedLimit;
    Polyline polyline;

    // Constructors
    Edge() : from(0), to(0), speedLimit(0) {}
    Edge (int from, int to, double speedLimit, const vector<Point> &container) :
        from(from), to(to), speedLimit(speedLimit), polyline(container) {}
    Edge (int from, int to, double speedLimit, const Polyline &init) :
        from(from), to(to), speedLimit(speedLimit), polyline(init) {}
    
    // Extract a portion of the polyline using the projection of a Point to itself
    Polyline extract (const Point &p, bool toEnd) { return polyline.extract(p, toEnd); }

    // Reverse the current edge
    Edge returnReverse() const { return Edge(to, from, speedLimit, polyline.returnReverse()); }
    void inlineReverse() { swap(from, to), polyline.inlineReverse(); }

    // Return edge's length
    double length() const { return polyline.length(); }
};

struct Graph {
    vector<vector<int>> adj;
    vector<Edge> edges;
    int graphSize;

    // Constructors
    Graph() : graphSize(0) {}
    Graph (int N, const vector<Edge> &varEdge, const vector<bool> oneway) : adj(N + 1), graphSize(N) {
        for (int i = 0; i < varEdge.size(); i++) {
            adj[varEdge[i].from].push_back(edges.size());
            edges.push_back(varEdge[i]);
            if (!oneway[i]) {
                adj[varEdge[i].to].push_back(edges.size());
                edges.push_back(varEdge[i].returnReverse());
            }
        }
    }
    
    // Return the geometric information of a path given its edge IDs
    Polyline mapEdgePath (const vector<int> &edgePath) {
        Polyline base;
        for (int u : edgePath) base += edges[u].polyline;
        return base;
    }

    // Return size of graph
    int size() { return graphSize; }
    int edgeCount() { return edges.size(); }
};