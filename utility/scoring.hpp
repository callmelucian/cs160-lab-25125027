#include <bits/stdc++.h>
#include "dijkstra.hpp"
using namespace std;

double normalDistribution (double gap, double variance) {
    return exp(-squared(gap) / (2 * squared(variance)));
}

namespace spatial {
    const double stdDeviation = 50;

    double observationProbability (const Graph &G, const CandidatePoint &cp) {
        double distanc = euclideDist(G.edges[cp.assocEdge].polyline, cp.gps), scalar = 1.0;
        double expo = normalDistribution(distanc, stdDeviation);
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
    const double largeVariance = 30;
    const double smallVariance = 5;

    double score (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
        double timeLimit = fastTravelTime(G, from, to);
        double actualTime = to.recordTime - from.recordTime;
        if (actualTime < timeLimit)
            return normalDistribution(timeLimit - actualTime, largeVariance);
        return normalDistribution(actualTime - timeLimit, smallVariance);
    }
};

double SPFunction (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
    return spatial::score(G, from, to) * temporal::score(G, from, to); // combining spatial & temporal analysis function
}