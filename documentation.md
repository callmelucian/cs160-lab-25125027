# Implementation of "Map-Matching for Low-Sampling-Rate GPS Trajectories" by Yin Lou et al.

## Simplified problem

Given a graph $G = (V, E)$ and a GPS trajectory $P_1, P_2, P_3, \dots, P_K$, return a map-matched trajectory $P'_1, P'_2 P'_3, \dots, P'_K$.

### Input

- The first line contains three intgers $N, M, K$ describes the number of nodes, edges and the size of the GPS trajectory.
- For the next $2M$ lines, each $2$ lines describe an edge in the following format:
  - The first line contains $2$ integers $u, v$, $1$ real number $\texttt{limit}$ and a boolean flag $\texttt{isOneway}$ (where $\texttt{isOneway} \in \{0, 1\}$), indicating its endpoints, the speed-limit of an edge and its type (uni-directional or bi-directional).
  - The second line begins with an integer $\ell$, then $2\ell$ real numbers follow. Each pair of real numbers describes a point of the polyline of the corresponding in the that order.
- For the next $K$ lines, each line contains $3$ real numbers describing the coordinate of $P_i$ and the corresponding recorded time.

### Output

- Output the list of points $P'_1, P'_2, P'_3, \dots, P'_K$ on $K$ seperate lines.

# Running pipeline

1. Function `load-data` to load:
    - HCMC Street Map from OSM: As 2 GeoDataFrames of the edges.
    - Raw GPS Trajectory: As a list of GeoDataFrames with corresponding ID.
    - Ground truth: As a list of GeoDataFrames with corresponding ID.

2. Function `evaluate(index)` to prepare input and run:
    - Prepare `input.txt` by accessing to the `index`-th GPS trajectory from the loaded data
    - Run `map-matching.exe` and receive `output.txt`.
    - Acesss to the corresponding ground truth and evaluate accuracy.

3. Run `evaluate.py` to perform all the task and return the average accuracy of the algorithm (to run full evaluation, set `maxIter` to an arbitrary large number).

# Improving time complexity of the algorithm

To the time complexity of algorithm, we identify the $2$ bottleneck of the original implementation:

- The cost of finding the candidate points using a naive algorithm is extremely large, having a time complexity of $\mathcal{O}(|K| M \log M)$. Although the theoretical time complexity of this step is smaller than Dijkstra queries, this step turned out to be the biggest bottleneck of the algorithm. For input size used in this research, specifically $|K| \approx 100$ and $M \approx 5 \cdot 10^5$, finding candidate points itself may consumes up to $10$ seconds!
- Running Dijkstra queries is also problematic because everytime the `SPFunction` is called, Dijkstra is also called (which runs on the whole graph). We can see that `SPFunction` is called exactly $(|K| - 1) \cdot \texttt{MAXCAND}^2$ times. Thus, the total time complexity for this part is $\mathcal{O}(|K| \cdot \texttt{MAXCAND}^2 \cdot M \log M)$, since an individual Dijkstra call costs $\mathcal{O}(M \log M)$. Turns out the actual running time is considerably faster, since Dijkstra usally terminates before scanning through the whole graph. However, we are going to optimize the time complexity of running Dijkstra, too.

## Finding candidate points

We observe that in the following naive algorithm for finding candidate points for each of the raw GPS input point:

```cpp
vector<vector<CandidatePoint>> candidates(K);
for (int i = 0; i < K; i++) {
    vector<pair<double,int>> distances(graph.edgeCount());
    for (int eID = 0; eID < graph.edgeCount(); eID++)
        distances[eID] = make_pair(graph.edges[eID].euclideDistance(trajectory[i]), eID);
    sort(distances.begin(), distances.end()); // too slow!

    vector<CandidatePoint> vec;
    for (int j = 0; j < graph.edgeCount() && j < MAXCANDIDATE; j++) {
        int edgeID = distances[j].second;
        vec.push_back(CandidatePoint(trajectory[i], edgeID, graph.edges[edgeID], recTime[i]));
    }
    candidates[i] = vec;
}
```

We are sorting the whole `distances` list just to get $\texttt{MAXCAND}$ smallest value! This caused us an extra, excessive $\log M$ factor in the time complexity. Cutting the log factor off could speed up the algorithm approximately $20$ times!

In this research, I propose an alternative, yet simple enough algorithm that cuts off the costly log factor as described. Let's say we randomly sample $U \geq \texttt{MAXCAND}$ edges in the edges list, then, find the $\texttt{MAXCAND}$-th closest edge (using sorting), let's call it $E_0$.

An important observation is that all true edge-candidates must be at least as close as $E_0$, otherwise, it would be eliminate at the first place. In other words, from now on, all edges further than $E_0$ will **not** be considered for sorting.

To estimate the number of "candidates" left in the edges list, we solve the following classic statistic problem: If we have a list of $M$ integers from $1$ to $M$ and sample $U$ distinct integers. What would be the expected $X$-th smallest integer sampled?

The answer to this problem is examined by statistic theory, thus, I will skip the proof in this research:

$$
E[T_{(X)}] = X \cdot \frac{M + 1}{U + 1}
$$

Thus, the ratio of the number of "candidates" left after this sampling step comparing to the original edges list is:

$$
R = \frac{\texttt{MAXCAND} \cdot \frac{M + 1}{U + 1}}{M} = \frac{\texttt{MAXCAND} \cdot (M + 1)}{U + 1} \cdot \frac{1}{M}
$$

For large $M$, the value for $R$ can be approximate as:

$$
R = \frac{\texttt{MAXCAND} \cdot (M + 1)}{U + 1} \cdot \frac{1}{M} \approx \frac{\texttt{MAXCAND}}{U + 1}
$$

By choosing $U$ to be twice as large as $\texttt{MAXCAND}$, after each sampling round, we can reduce the number of "candidates" by a half. In other words, if we repeatedly perform the described sampling procedure until the size of the original edges list reduces from $M$ to $\texttt{MAXCAND}$, the expected number of round it would take is:

$$
E[\texttt{step}] = \log \left( \frac{M}{\texttt{MAXCAND}} \right) \approx \log M
$$

Each sampling round takes $\mathcal{O}(U \log U)$ time complexity, and since $U = 2 \cdot \texttt{MAXCAND}$, we can reduce the time complexity of the whole process to:

$$
\mathcal{O}(|K| \cdot M + \texttt{MAXCAND} \log \texttt{MAXCAND} \log M)
$$

Here $\texttt{MAXCAND}$ is extremely small comparing to $|K| \cdot M$, even if we consider the 2 log factors, thus, the time complexity can be treated as a linear complexity $\mathcal{O}(|K| \cdot M)$.

## Dijkstra queries

We can observe that most Dijkstra queries called in the algorithm happens between 2 relatively close nodes. Specifically, for $\texttt{MAXCAND} = 6$, the following bar graph indicates the number of Dijkstra calls per iterations before the function terminates:

![image](/analysis/Figure%201.png)

Thus, a simple precomputation that runs the Dijkstra for $100$ closest nodes from each source could significantly reduces the running time of Dijkstra, while being able to maintain all required information.

In order to reduce the memory size of the precomputation data, I have used binary file to store the Dijkstra results (in `UBODT.bin`).