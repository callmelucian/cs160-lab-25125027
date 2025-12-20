#include <bits/stdc++.h>
#include "dijkstra.h"
using namespace std;

const double pi = acos(-1);
const double stdDeviation = 20;

/// SPATIAL ANALYSIS FUNCTIONS
double observationProbability (const CandidatePoint &point) {
    double scalar = 1.0;// 1.0 / (stdDeviation * sqrt(2 * pi));
    double expo = exp(-squaredDist(point.assocPoint, point.gps) / (2 * squared(stdDeviation)));
    return scalar * expo;
}

double transmissionProbability (const Graph &graph, const CandidatePoint &from, const CandidatePoint &to) {
    double shortestPath = dijkstra(graph, from, to, 0).length;
    double euclid = euclideDist(from.gps, to.gps);
    // this formula is from Can Yang's paper since Yin Lou's formula left some problematic bugs
    double probability = min(shortestPath, euclid) / max(shortestPath, euclid);
    return probability;
}

double spatialAnalysis (const Graph &graph, const CandidatePoint &from, const CandidatePoint &to) {
    return observationProbability(to) * transmissionProbability(graph, from, to);
}

/// TEMPORAL ANALYSIS FUNCTIONS
double temporalAnalysis (const Graph &graph, const CandidatePoint &from, const CandidatePoint &to) {
    // precomputations
    assert(from.recordTime < to.recordTime);
    Path shortestPath = dijkstra(graph, from, to, 1);
    double averageVelocity = 1;//shortestPath.length / (to.recordTime - from.recordTime);

    // dot product computations
    double dotProduct = 0.0;
    for (int eID : shortestPath.edgeID)
        dotProduct += graph.edges[eID].speedLimit;
    dotProduct *= averageVelocity;

    // vector lengths computations
    double lenE = 0.0;
    for (int eID : shortestPath.edgeID)
        lenE += squared(graph.edges[eID].speedLimit);
    lenE = sqrt(lenE);
    double lenV = sqrt(squared(averageVelocity) * shortestPath.edgeID.size());

    // return cosine similarity
    return dotProduct / (lenE * lenV);
}

double SPFunction (const Graph &graph, const CandidatePoint &from, const CandidatePoint &to) {
    return spatialAnalysis(graph, from, to) * temporalAnalysis(graph, from, to);
}