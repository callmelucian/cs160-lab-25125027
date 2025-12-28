#include <bits/stdc++.h>
#include "scoring.h"
using namespace std;

vector<int> dynamicProgramming (const Graph &graph, vector<vector<CandidatePoint>> &candidates) {
    /// intialize helper vectors
    vector<vector<double>> dp(1, vector<double>(candidates[0].size()));
    vector<vector<int>> trace(1);

    /// run DP
    for (int layer = 1; layer < candidates.size(); layer++) {
        vector<CandidatePoint> candList = candidates[layer];
        dp.push_back(vector<double>(candList.size(), -oo));
        trace.push_back(vector<int>(candList.size()));

        for (int i = 0; i < candList.size(); i++) {
            for (int j = 0; j < candidates[layer - 1].size(); j++) {
                CandidatePoint p1 = candidates[layer - 1][j], p2 = candList[i];
                double tryVal = dp[layer - 1][j] + SPFunction(graph, p1, p2);
                if (tryVal > dp[layer][i]) dp[layer][i] = tryVal, trace[layer][i] = j;

                // cerr << "cost of " << p1.gps << " " << p1.assocPoint << " " << p2.gps << " " << p2.assocPoint << " ";
                // cerr << layer - 1 << j << " " << layer << i << " " << SPFunction(graph, p1, p2) << endl;
            }
        }
    }

    /// trace
    vector<int> ans;
    int cur = max_element(dp.back().begin(), dp.back().end()) - dp.back().begin();
    for (int layer = (int)candidates.size() - 1; layer > 0; layer--) {
        ans.push_back(cur);
        cur = trace[layer][cur];
    }
    ans.push_back(cur);
    reverse(ans.begin(), ans.end());
    return ans;
}