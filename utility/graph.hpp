#include <bits/stdc++.h>
#include "geometry.hpp"
using namespace std;

mt19937 rng(21); // shuffling the edge IDs must be consistent

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

/// GPS CANDIDATE POINT
struct CandidatePoint {
    double recordTime;
    int assocEdge;
    Point gps;

    CandidatePoint() : assocEdge(0) {}
    CandidatePoint (Point raw, int edgeID, double recordTime) :
        gps(raw), recordTime(recordTime), assocEdge(edgeID) {}
};

/// GRAPH
struct Graph {
    vector<vector<int>> adj;
    vector<Edge> edges;
    int graphSize;

    // Constructors
    Graph() : graphSize(0) {}
    Graph (int N, const vector<Edge> &varEdge, const vector<bool> oneway) : adj(N + 1), graphSize(N) {
        // prepare random order
        vector<int> ord(varEdge.size());
        iota(ord.begin(), ord.end(), 0);
        shuffle(ord.begin(), ord.end(), rng);

        // prepare real edges
        for (int i : ord) {
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
    int size() const { return graphSize; }
    int edgeCount() const { return edges.size(); }
};

/// @brief Utility function to read a graph in 2 modes
/// @param fileName Relative path to graph file location
/// @param mode For now, I only accept "txt" and "bin" mode, binary files are recommended since they are generally faster
/// @return Return a Graph structure corresponding to the given file
Graph readGraph (string fileName, string mode) {
    if (mode == "txt") {
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

            edges[i] = Edge(u, v, speedLimit * 1000.0 / 60.0, pll);
            isOneway[i] = oneway;
        }
        fin.close();
        return Graph(N, edges, isOneway);
    }
    else if (mode == "bin") {
        ifstream fin(fileName, ios::binary);
        int N; fin.read((char*)&N, 4); // nodes count
        int M; fin.read((char*)&M, 4); // edges count
        vector<Edge> edges(M);
        vector<bool> isOneway(M);

        for (int i = 0; i < M; i++) {
            int u; fin.read((char*)&u, 4); // edge endpoint
            int v; fin.read((char*)&v, 4); // edge endpoint
            double speedLimit; fin.read((char*)&speedLimit, 8); // speed-limit
            int oneway; fin.read((char*)&oneway, 4); // is-oneway flag

            int polylength; fin.read((char*)&polylength, 4); // poly-line length
            vector<Point> pll(polylength);
            for (Point &it : pll) {
                fin.read((char*)&it.x, 8); // point x-coordinate
                fin.read((char*)&it.y, 8); // point y-coordinate
            }

            edges[i] = Edge(u, v, speedLimit * 1000.0 / 60.0, pll);
            isOneway[i] = oneway;
        }
        fin.close();
        return Graph(N, edges, isOneway);
    }
    else return Graph();
}

vector<CandidatePoint> readTrajectory (string fileName, string mode) {
    if (mode == "txt") {
        // cout << "Start reading trajectory..." << endl;
        ifstream fin(fileName);
        int K; fin >> K;
        vector<CandidatePoint> candList(K);
        for (CandidatePoint &cand : candList)
            fin >> cand.gps >> cand.recordTime;
        fin.close();
        return candList;
    }
    return vector<CandidatePoint>(0);
}

void writeMatchedTrajectory (string fileName, string mode, const Polyline &p) {
    // cout << "Start writing matched trajectory..." << endl;
    ofstream fout(fileName);
    fout << p.size() << "\n";
    fout << fixed << setprecision(9);
    for (int i = 0; i < p.size(); i++) fout << p[i].x << " " << p[i].y << "\n";
    fout.close();
}