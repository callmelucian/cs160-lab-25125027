/**
 * This file is for performing unit tests on modules
 * before combining them into the final program
 */

#include <bits/stdc++.h>
// #include "../utility/dijkstra.h"
#include "../utility/scoring.h"
using namespace std;

using ll = long long;
using ld = long double;
using pl = pair<ll,ll>;
using pii = pair<int,int>;
using tpl = tuple<int,int,int>;

#define all(a) a.begin(), a.end()
#define filter(a) a.erase(unique(all(a)), a.end())

namespace dijkstraTest {
    void mainFunction() {
        // initialize edges
        int N, M, K; cin >> N >> M >> K;
        vector<Edge> edges(M);

        for (int i = 0; i < M; i++) {
            int u, v, isOneway; double limit; cin >> u >> v >> limit >> isOneway;
            int polySize; cin >> polySize;
            vector<Point> polyline(polySize);
            for (Point &p : polyline) cin >> p;
            edges[i] = Edge(u, v, limit, polyline, isOneway);
        }
        Graph graph(N, edges);

        for (int i = 0; i < graph.edges.size(); i++) {
            cout << i << " " << graph.edges[i].start << " --> " << graph.edges[i].end << "\n";
            // for (Point p : graph.edges[i].polyline) cout << p << " ";
            // cout << "\n";
        }

        // initialize candidate-point
        Point rawStart(1.78, 7.9), rawEnd(4, 12.3);
        CandidatePoint pStart(rawStart, 9, graph.edges[9], 50.0);
        CandidatePoint pEnd(rawEnd, 21, graph.edges[21], 50.0);
        Path shortestPath = dijkstra(graph, pStart, pEnd, 1);
        // return;
        // cout << shortestPath << "\n";

        cout << observationProbability(pEnd) << " " << transmissionProbability(graph, pStart, pEnd) << "\n";
        cout << spatialAnalysis(graph, pStart, pEnd) << "\n";
    }
};

namespace geometryTest {
    void mainFunction() {
        cout << "Geometry test:\n";
        Edge edge(0, 0, 0.0, {
            Point(21, 8), Point(24, 10), Point(27, 9), Point(32, 11), Point(34, 6), Point(29, 7)
        }, 0);
        cout << edge.edgeProjection(Point(19, 9)) << " ";
        cout << edge.edgeProjection(Point(27, 8)) << " ";
        cout << edge.edgeProjection(Point(28, 12)) << " ";
        cout << edge.edgeProjection(Point(31, 8)) << " ";
        cout << edge.edgeProjection(Point(33, 12)) << " ";
        cout << edge.edgeProjection(Point(35, 9)) << "\n";
    }
};

namespace evaluateTest {
    void mainFunction() {
        Path path1({Point(0, 0), Point(3, 4), Point(9, 6), Point(10, 10), Point(14, 12)}, {});
        Path path2({Point(0, 0), Point(3, 1), Point(0, 3), Point(7, 16.0 / 3.0), Point(9, 8), Point(9.5, 8), Point(9.75, 9), Point(8, 9), Point(14, 12)}, {});
        cout << overlapRatio(path1, path2) << "\n";

        // Segment a(Point(3, 4), Point(9, 6)), b(Point(0, 3), Point(7, 16.0 / 3.0));
        // cout << intersectLength(a, b) << "\n";
    }
};

int main()
{
    freopen("../input.txt", "r", stdin);
    ios::sync_with_stdio(0);
    cin.tie(0);

    // geometryTest::mainFunction(); // correct
    // dijkstraTest::mainFunction(); // correct
    evaluateTest::mainFunction();

    return 0;
}