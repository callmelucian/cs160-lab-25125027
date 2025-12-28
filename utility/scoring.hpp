#include <bits/stdc++.h>
#include "dijkstra.hpp"
using namespace std;

/// @brief Calculate the normal-distribution function exp(-gap^2 / (2 * variance))
/// @param gap Actual gap
/// @param variance Expected gap
/// @return The value of the mentioned function
long double normalDistribution (long double gap, long double variance) {
    return exp(-squared(gap) / (2 * squared(variance)));
}

/// @brief Compute spartial analysis functions
namespace spatial {
    const double stdDeviation = 50;

    long double observationProbability (const Graph &G, const CandidatePoint &cp) {
        long double distanc = euclideDist(G.edges[cp.assocEdge].polyline, cp.gps), scalar = 1.0;
        long double expo = normalDistribution(distanc, stdDeviation);
        return scalar * expo;
    }

    long double transmissionProbability (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
        long double shortestPath = shortestPathLength(G, from, to);
        long double euclid = euclideDist(from.gps, to.gps);
        // This formula is from Can Yang's paper since Yin Lou's formula left some problematic bugs
        long double probability = min(shortestPath, euclid) / max(shortestPath, euclid);
        return probability;
    }

    long double score (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
        return observationProbability(G, to) * transmissionProbability(G, from, to);
    }
};

/// @brief Compute temporal analysis functions
namespace temporal {
    const double largeVariance = 40;
    const double smallVariance = 20;

    long double score (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
        long double timeLimit = fastTravelTime(G, from, to);
        long double actualTime = to.recordTime - from.recordTime;
        if (actualTime < timeLimit)
            return normalDistribution(timeLimit - actualTime, largeVariance);
        return normalDistribution(actualTime - timeLimit, smallVariance);
    }
};

/// @brief Calculate the scoring of moving between 2 candidate points according to the paper
/// @param G Input graph
/// @param from Starting candidate point
/// @param to Ending candidate point
/// @return The scoring according to the paper
long double SPFunction (const Graph &G, const CandidatePoint &from, const CandidatePoint &to) {
    return spatial::score(G, from, to) * temporal::score(G, from, to); // combining spatial & temporal analysis function
}