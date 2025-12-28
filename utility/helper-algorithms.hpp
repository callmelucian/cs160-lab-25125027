#include <bits/stdc++.h>
#include "scoring.hpp"
using namespace std;

/// @brief Find the best scoring path on the virtual graph
/// @param G Input graph
/// @param candList The list of candidates for each layer of the virtual graph
/// @return The list of candidates' ID of the optimal path
vector<int> bestScoringPath (const Graph &G, const vector<vector<CandidatePoint>> &candList) {
    // pre-process
    for (const vector<CandidatePoint> &cands : candList) {
        if (cands.empty()) throw invalid_argument("Found empty layer of candidates list");
        if (cands.size() != candList[0].size()) throw invalid_argument("Inconsistent layer size");
    }
    vector<vector<double>> bestScore(candList.size(), vector<double>(candList[0].size(), 0.0));
    vector<vector<int>> trace(candList.size(), vector<int>(candList[0].size(), 0));

    // perform dynamic programming
    for (int layer = 1; layer < candList.size(); layer++) {
        for (int i = 0; i < candList[layer - 1].size(); i++) {
            for (int j = 0; j < candList[layer].size(); j++) {
                double score = SPFunction(G, candList[layer - 1][i], candList[layer][j]);
                if (bestScore[layer - 1][i] + score > bestScore[layer][j]) {
                    bestScore[layer][j] = bestScore[layer - 1][i] + score;
                    trace[layer][j] = i;
                }
            }
        }
    }

    // trace for actual path
    int arcmax = max_element(bestScore.back().begin(), bestScore.back().end()) - bestScore.back().begin();
    int currentLayer = bestScore.size() - 1;
    vector<int> foundPath = {arcmax};
    while (currentLayer > 0) {
        int prevOpt = trace[currentLayer][arcmax];
        foundPath.push_back(prevOpt);
        currentLayer--, arcmax = prevOpt;
    }
    reverse(foundPath.begin(), foundPath.end());
    return foundPath;
}

/// @brief Shrink a vector until its size is less than or equal a given THRESHOLD
/// @param vec Target vector
/// @param THRESHOLD Threshold
void reshrink (vector<pair<double,int>> &vec, int THRESHOLD) {
    sort(vec.begin(), vec.end());
    while (vec.size() > THRESHOLD) vec.pop_back();
}

/// @brief Search for the THRESHOLD-closest edges (in terms of Euclidean distance) from p
/// @param G Input graph
/// @param p Target point
/// @param THRESHOLD Threshold for the number of returned edges
/// @return Return a list of edgeID not neccessarily sorted
vector<int> searchXBest (const Graph &G, const Point &p, int THRESHOLD) {
    // intialize
    vector<pair<double,int>> waitLine;
    double currentBound = oo;

    // start sampling rounds
    for (int i = 0; i < G.edgeCount(); i++) {
        double curDist = squaredDist(G.edges[i].polyline, p); // used squared distance for faster calculations
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

/// @brief Find the candidate list for each point on the raw GPS trajectory
/// @param G Input list
/// @param rawList Raw GPS trajectory
/// @param THRESHOLD Threshold for the number of returned edges for each point
/// @return Return a vector of candidate list
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