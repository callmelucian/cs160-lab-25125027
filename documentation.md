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