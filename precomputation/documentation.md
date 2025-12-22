# Precomputations

To speedup the process of running Dijkstra, we may use a simple observation that the distance difference between $2$ queried nodes during the process of running Map Matching is relatively small. Thus, for each node (source), we can run Dijkstra for $\texttt{thres}$ closest nodes, where $\texttt{thres}$ is a fixed constant. For HCMC map where the number for nodes are $2 \cdot 10^5$, choosing $\texttt{thres} = 75$ would only take less than $5$ seconds to process, and only require an additional $86 \text{ MB}$, if we use binary file and use only $3 \text{ bytes}$ for each integer!

The Dijkstra query $u \rightarrow v$ could be splitted into $2$ cases:
1. **When $v$ is within the $\texttt{thres}$ closest node from $u$:** We simply trace the shortest path from $u$ to $v$ by reading the neccessary information from the binary file.
2. **Or else:** We run the original Dijkstra query again, however, we can see that this situation is rare.

## Binary file structure (`UBODT.bin`)

### Preparing (implemented in `build-UBODT.cpp`)

For each $\texttt{source}$ node from $1$ to $N$, we will use exactly $6 \cdot \texttt{thres}$ bytes of the binary file. To do this, run Dijkstra with $\texttt{source}$ and terminate when the $\texttt{thres}$-th node is visited.

The, we record the visited node **in that order** with a value $\texttt{trace}[v]$ to indicate that in the shortest path from $\texttt{source}$ to $v$, the node $\texttt{trace}[v]$ would come right before $v$. That is sufficient to trace the shortest path from $\texttt{source}$ to any of the visited nodes.

### Reading

When a Dijkstra query $u \rightarrow v$ is called, we simply look for the block of $6 \cdot \texttt{thres}$ bytes, starting from the $(6 \cdot \texttt{thres}) \cdot (u - 1)$-th byte. After data extraction, we will lookup for $v$ and process the query as described above.