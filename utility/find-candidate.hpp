#include <bits/stdc++.h>
#include "../utility/graph.hpp"
using namespace std;

void reshrink (vector<pair<double,int>> &vec, int THRESHOLD) {
    sort(vec.begin(), vec.end());
    while (vec.size() > THRESHOLD) vec.pop_back();
}

vector<int> searchXBest (const Graph &G, const Point &p, int THRESHOLD) {
    // intialize
    vector<pair<double,int>> waitLine;
    double currentBound = oo;

    // start sampling rounds
    for (int i = 0; i < G.edgeCount(); i++) {
        double curDist = euclideDist(G.edges[i].polyline, p);
        if (curDist <= currentBound) waitLine.emplace_back(curDist, i);

        // re-calculate wait-line, shrink to THRESHOLD elements
        // and re-calculate current bound
        if (waitLine.size() == THRESHOLD * 2) {
            reshrink(waitLine, THRESHOLD);
            currentBound = waitLine.back().first;
        }
    }

    // refine answer
    if (waitLine.size() > THRESHOLD) reshrink(waitLine, THRESHOLD);
    vector<int> result;
    result.reserve(waitLine.size());
    for (auto [dist, id] : waitLine) result.push_back(id);
    return result;
}

vector<vector<CandidatePoint>> findCandidates (const Graph &G, const vector<CandidatePoint> &rawList, int THRESHOLD) {
    int K = rawList.size();
    vector<vector<CandidatePoint>> candidates(K);
    for (int i = 0; i < K; i++) {
        vector<int> candidateEdges = searchXBest(G, rawList[i].gps, THRESHOLD); // this is where the magic happens
        assert(candidateEdges.size() == THRESHOLD);
        vector<CandidatePoint> vec;
        for (int edgeID : candidateEdges) {
            CandidatePoint newPoint = rawList[i];
            newPoint.assocEdge = edgeID;
            vec.push_back(newPoint);
        }
        candidates[i] = vec;
    }
    return candidates;
}