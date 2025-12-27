#include <bits/stdc++.h>
#include "dijkstra.hpp"
using namespace std;

namespace spatial {
    const double stdDeviation = 50;

    double observationProbability (const Graph &G, const CandidatePoint &cp) {
        double distanc = euclideDist(G.edges[cp.assocEdge].polyline, cp.gps), scalar = 1.0;
        double expo = exp(-squared(distanc) / (2 * squared(stdDeviation)));
        return scalar * expo;
    }

    double transmissionProbability (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
        double shortestPath = shortestPathLength(G, from, to);
        double euclid = euclideDist(from.gps, to.gps);
        // This formula is from Can Yang's paper since Yin Lou's formula left some problematic bugs
        double probability = min(shortestPath, euclid) / max(shortestPath, euclid);
        return probability;
    }

    double score (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
        return observationProbability(G, to) * transmissionProbability(G, from, to);
    }
};

namespace temporal {
    double score() { return 1.0; }
};

double SPFunction (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
    return spatial::score(G, from, to) * temporal::score();
}