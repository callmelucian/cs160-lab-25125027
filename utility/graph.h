#include <bits/stdc++.h>
#include "geometry.hpp"
using namespace std;

/// PATH ON MAP
struct Path {
    double length;
    vector<Point> polyline;
    vector<int> edgeID;

    Path() : length(0) {}
    Path (double length) : length(length) {}
    Path (const vector<Point> &polyline, const vector<int> &edgeID) :
        length(polyLength(polyline)), polyline(polyline), edgeID(edgeID) {}

    void append (const Point &p) {
        if (polyline.size() && polyline.back() == p) return;
        if (polyline.size()) length += euclideDist(polyline.back(), p);
        polyline.push_back(p);
    }

    void addEdge (int eID) {
        if (edgeID.empty() || edgeID.back() != eID) edgeID.push_back(eID);
    }

    void reversePath() {
        reverse(polyline.begin(), polyline.end());
        reverse(edgeID.begin(), edgeID.end());
    }

    void extendBack (const Path &o) {
        for (auto it = o.polyline.begin(); it != o.polyline.end(); ++it) append(*it);
        for (auto it = o.edgeID.begin(); it != o.edgeID.end(); ++it) addEdge(*it);
    }

    void extendBack (const vector<Point> &pll) {
        for (Point p : pll) append(p);
    }

    void extendFront (const Path &o) {
        reversePath();
        for (auto it = o.polyline.rbegin(); it != o.polyline.rend(); ++it) append(*it);
        for (auto it = o.edgeID.rbegin(); it != o.edgeID.rend(); ++it) addEdge(*it);
        reversePath();
    }

    Segment getSegment (int i) const { return Segment(polyline[i], polyline[i + 1]); }

    friend ostream& operator << (ostream &ouf, const Path &p) {
        ouf << "<Path with length " << p.length << ": ";
        for (Point pt : p.polyline) ouf << pt << " ";
        ouf << "/ ";
        for (int u : p.edgeID) ouf << u << " ";
        ouf << "/>";
        return ouf;
    }

    friend double overlapRatio (const Path &a, const Path &b) {
        double intersection = 0.0;
        cerr << "trying " << a.size() << " " << b.size() << "\n";
        for (int i = 0; i + 1 < a.size(); i++)
            for (int j = 0; j + 1 < b.size(); j++)
                intersection += intersectLength(a.getSegment(i), b.getSegment(j));
        double total = a.length + b.length, unon = total - intersection;
        cerr << intersection << " " << unon << "\n";
        return intersection / unon;
    }

    int size() const { return polyline.size(); }
};

/// EDGE ON GRAPH
struct Edge {
    int start, end;
    double length, speedLimit;
    vector<Point> polyline;
    bool isOneway;

    Edge() : start(0), end(0), length(0), isOneway(0) {}
    Edge (int start, int end, double speedLimit, const vector<Point> &polyline, bool isOneway) :
        start(start), end(end), length(polyLength(polyline)), speedLimit(speedLimit), polyline(polyline), isOneway(isOneway) {}
    
    Path startToPoint (const Point &p) const {
        Path curPath; curPath.append(polyline[0]);
        for (int i = 0; i + 1 < polyline.size(); i++) {
            if (pointOnLine(polyline[i], polyline[i + 1], p)) {
                curPath.append(p);
                return curPath;
            }
            curPath.append(polyline[i + 1]);
        }
        return curPath;
    }

    Path pointToEnd (const Point &p) const {
        Path curPath; curPath.append(polyline.back());
        for (int i = (int)polyline.size() - 1; i > 0; i--) {
            if (pointOnLine(polyline[i], polyline[i - 1], p)) {
                curPath.append(p);
                return curPath.reversePath(), curPath;
            }
            curPath.append(polyline[i - 1]);
        }
        curPath.reversePath();
        return curPath;
    }

    Point edgeProjection (const Point &p) const {
        double minD = oo;
        Point bestProjection = polyline[0];

        for (int i = 0; i + 1 < polyline.size(); i++) {
            Segment seg(polyline[i], polyline[i + 1]);
            double d = euclideDist(seg, p);
            if (d < minD) minD = d, bestProjection = projection(seg, p);
        }
        return bestProjection;
    }

    double euclideDistance (const Point &p) const {
        double ans = euclideDist(Segment(polyline[0], polyline[1]), p);
        for (int i = 1; i + 1 < polyline.size(); i++)
            ans = min(ans, euclideDist(Segment(polyline[i], polyline[i + 1]), p));
        return ans;
    }

    void reversePolyline() {
        reverse(polyline.begin(), polyline.end());
        swap(start, end);
    }
};

/// GPS CANDIDATE POINT
struct CandidatePoint {
    Point assocPoint, gps;
    double recordTime, distanc;
    int assocEdge;

    CandidatePoint() : assocEdge(0) {}
    CandidatePoint (Point raw, int edgeID, const Edge &e, double recordTime) :
        assocPoint(e.edgeProjection(raw)), gps(raw), recordTime(recordTime), assocEdge(edgeID), distanc(euclideDist(e.edgeProjection(raw), gps)) {}
};

/// GRAPH STRUCT
struct Graph {
    vector<vector<int>> adj;
    vector<Edge> edges;
    int graphSize;

    Graph() : graphSize(0) {}
    Graph (int n, vector<Edge> &varEdges) : adj(n + 1), edges(varEdges), graphSize(n) {
        int m = edges.size(), countTwoway = 0;
        for (int i = 0; i < m; i++) countTwoway += (!edges[i].isOneway);
        edges.reserve(m + countTwoway);

        for (int i = 0; i < m; i++) {
            // forward edges
            adj[edges[i].start].push_back(i);
            // backward edges
            if (!edges[i].isOneway) {
                Edge currentEdge = edges[i];
                currentEdge.reversePolyline();
                adj[currentEdge.start].push_back(edges.size());
                edges.push_back(currentEdge);
            }
        }
    }

    int size() const { return graphSize; }
    int edgeCount() const { return edges.size(); }
};