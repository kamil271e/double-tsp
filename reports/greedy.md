# Greedy
All algorithms start by selecting the initial vertices for both cycles - the ```start_vertices()``` function is used for this purpose. 
The first of these vertices is chosen in two ways - as an input argument of the program (if any) or randomly - sampling from a uniform distribution, while the second is chosen to be as far as possible from the first. 
The two vertices are then added to two different cycles: cycle1 and cycle2 - which are represented as vectors of the corresponding vertices. 
In addition, the pseudocode uses the helper functions ```insert_vertex(i, pos, cycle)``` that inserts a vertex with index ```i``` into the ```pos``` positions in ```cycle ```
and ```append_vertex(i, cycle)``` that adds a vertex to the end of the path or between the "last" and "first" elements in cycle.

## Nearest Neighbour
The main loop of the algorithm executes until both cycles contain all the vertices. 
In each iteration of the loop, for both cycles using the ```find_nearest_neighbor()``` function iterating over the distance matrix, the nearest neighbors of the last and first vertices of the current path are selected and
the one whose distance is minimal is chosen from them - the nearest variables. Then the selected nearest neighbor is added to the shorter of the cycles (or cycle 1 if they are equal), taking into account the place of insertion
(at the end or beginning of the path being built). The variables representing the final vertices in both cycles are updated.

#### Pseudocode
```rust
fun greedy_nearest():
    start = start_vertices()
    last_v1, first_v1 = start[0], start[0]
    last_v2, first_v2 = start[1], start[1]
    append_vertex(last_v1, cycle1)
    append_vertex(last_v2, cycle2)

    while total_vertices_covered(cycle1, cycle2) < total_vertices():
        last_v, first_v = (last_v1, first_v1) 
                                        if cycle1.size() <= cycle2.size() else (last_v2, first_v2) 
        shorter_cycle = choose_shorter_cycle(cycle1, cycle2)
        nearest, type = find_nearest_neighbor(last_v, first_v, visited)

        if type == 0:
            append_vertex(nearest, shorter_cycle)
            last_v = nearest
        else:
            insert_vertex(nearest, 0, shorter_cycle)
            first_v = nearest

    return {cycle1, cycle2}

fun find_nearest_neighbor(last, first, visited):
    last_min = min(v.index, dist_matrix[v][last] for v in !visited)
    first_min = min(v.index, dist_matrix[v][first] for v in !visited)
    return last_min if (last_min[1] < first_min[1]) else first_min
```

## Greedy Cycles
The main loop of the algorithm executes until both cycles contain all the vertices.  The algorithm, as before, adds vertices to the cycles alternately, always choosing the cycle of shorter length (or cycle 1). 
The process of selecting cycle expansion is based on iterating over all pairs of neighboring vertices located in the considered cycle, finding for them the vertices having the smallest expansion cost and, from such a set of vertices, 
selecting the one with the lowest cost and, finally, inserting it between the corresponding pair of neighbors. The said cost is calculated on the basis of the distance matrix as the sum of the distances of neighboring vertices and the 
considered insertion candidate minus the distance between neighbors.

#### Pseudocode
```rust
fun greedy_cycles():
    start = start_vertices()
    append_vertex(start[0], cycle1)
    append_vertex(start[1], cycle2)

    while total_vertices_covered(cycle1, cycle2) < total_vertices():
        cycle = choose_shorter_cycle(cycle1, cycle2)
        min_length = infinity
        candidate, pos = -1

        for i in range(cycle.size()):
            nearest, length = cycle_expansion_cost(cycle[i], cycle[(i + 1) % cycle.size()], visited)
            if length < min_length:
                min_length = length
                candidate = nearest
                pos = i

        insert_vertex(candidate, (pos + 1) % cycle.size(), cycle)

    return {cycle1, cycle2}

fun cycle_expansion_cost(i, j, visited):
    min_idx, min_distance = min((v.index, dist_matrix[i][v] + dist_matrix[j][v] - dist_matrix[i][j]) for v in !visited)
    return min_idx, min_distance

```
## 2-Regret
As in previous cases, the main loop of the algorithm executes until both cycles contain all vertices. In each iteration, a 2-regret value is calculated for each unvisited vertex. 
Out of all possible places to insert a vertex, the 2 most profitable options are found - the ones that increase the cycle length the least (similar to the algorithm above), the 2-regret value is called the absolute difference between them.
In addition, the said regret value is weighted by an empirically determined ```coef```. The vertex inserted in a given iteration has the maximum value of weighted regret.

```rust
fun greedy_regret():
    start = choose_starting_vertices()
    append_vertex(start[0], cycle1)
    append_vertex(start[1], cycle2)
    
    regrets = empty map of (int, (int, double))
    costs = empty vector of (int, double)

    while total_vertices_covered(cycle1, cycle2) < total_vertices():
        cycle = choose_shorter_cycle(cycle1, cycle2)

        for i = 0 to total_vertices():
            if not visited[i]:
                costs = for all v in cycle : cycle_expansion_cost(v, i, cycle)
                min_costs = sorted(costs.items(), key=lambda x: x[1])[:2]     *
                regret = min_costs[1][1] - min_costs[0][1]
                weighted_regret = regret - coef * min_costs[0][1]
                regrets[i] = {min_costs[0], weighted_regret}

        max_regret = max(regrets.items(), key=lambda x: x[1][1])
        candidate = max_regret[0]
        pos = max_regret[1][0]
        insert_vertex(candidate, (pos + 1) % size(cycle), cycle)

    return {cycle1, cycle2}
```
*in the actual algorithm, we don't sort the cost collection but select the 2 smallest elements from it by reviewing it - for ease of pseudo-code we have changed the notation 

## Results
The results shown for A and B instances (kroA100.tsp kroB100.tsp), were obtained by executing each algorithm 100 times for each instance, selecting all possible starting vertices sequentially. The table below shows the sum of the longest, shortest and average cycle lengths.
|   Algorithm        | Min-A   | Mean-A  | Max-A   | Min-B   | Mean-B  | Max-B   |
|-----------|-------|-------|-------|-------|-------|-------|
| Nearest Neighbor | 25530 | 29514 | 31429 | 26227 | 30190 | 33480 |
| Greedy Cycle   | 25099 | 28721 | **30049** | 27180 | **28538** | **30197** |
| Biggest 2-regret  | **21988** | **26757** | 32573 |**25831** | 28775 | 32637 |

## Best cycles [kroA100.tsp | kroB100.tsp]

#### Nearest Neighbor
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/0f69d8ee-8bcd-48fe-993d-0b230ffc8a63" alt="BEST_nearest_kroA100" width="430"/> | <img src="https://github.com/kamil271e/double-tsp/assets/82380348/1e39df6a-82f1-4645-8d44-202f2fe9051c" alt="BEST_nearest_kroB100" width="430"/>

#### Greedy Cycles
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/ada37422-a82c-45ec-ad59-78347b9533d6" alt="BEST_expansion_kroA100" width="450"/> <img src="https://github.com/kamil271e/double-tsp/assets/82380348/4ecf3570-45af-4d85-8d10-79345bddb497" alt="BEST_expansion_kroB100" width="450"/>

#### 2-Regret
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/20a9601a-5578-46b6-92f5-8bab59fc58ff" alt="BEST_regret_kroA100" width="450"/> <img src="https://github.com/kamil271e/double-tsp/assets/82380348/a9f6c125-bed5-41e0-9de7-a3fb1bfb3e06" alt="BEST_regret_kroB100" width="450"/>



