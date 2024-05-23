# Local search

## Generate movements / neighbourhood
We made an assumption about the neighborhood, which allows us to represent each move as a tuple of three values ``(i, j, type)``, where ``i`` and ``j`` represent the indexes of the elements in the cycle(s), while ``type`` refers to the type of move being made, in turn ``0`` means a change of edges and ``1`` means a change of vertices. Accordingly, for all possibilities before the start of the algorithm, the aforementioned tuples were generated, taking into account special situations such as skipping the situation in which edges would be generated for the indexes of the first and last respectively - such a situation does not affect the change of the cycle structure in any way.

### Edge movements
```rust
fun generate_edge_movements():
    for i = 0 to cycle.size():
        for j = i + 2 to cycle.size():
            if i==0 && j==n-1: continue
            movements.add({i, j, 0})
    return movements
```
### Vertex movements: inner-class
```rust
fun generate_vertex_movements():
    for i = 0 to cycle.size():
        for j = 0 to cycle.size():
            if (i==j) continue;
            movements.add({i, j, 1})
    return movements
```
### Vertex movements: inter-class
```rust
fun generate_vertex_movements_inter():
    for i = 0 to cycle.size():
        for j = 0 to cycle.size():
            movements.add({i, j, 1})
    return movements
```

## Get objective value (delta)
In order to obtain the delta of the objective function, we had to perform operations to check whether the length of the edges needed to be removed when considering a given move, minus the length of the added edges, would be a value greater than 0 - such situations indicate a reduction in the length of our cycle after applying the operation.
Variables interpretation: <br>
``M``: distance matrix <br>
``i, i_left, i_right``
``j, j_left, j_right``: indices (for inter-class operations we make assumption that ``i`` always correspond to ``cycle1`` and ``j`` to ``cycle2``) <br>
``dist(i, j)``: distance between i-th and j-th element of the cycle / cycles <br>
```rust
i_left, i_right = (i - 1 + n) % n, (i + 1) % n;
j_left, j_right = (j - 1 + n) % n, (j + 1) % n;
lambda dist(i, j): M[cycle[i]][cycle[j]]
```

### inner-class
```rust
fun get_objective_value(movement):
    i, j, n = movement[0], movement[1], cycle.size()
    if movement[2] == 0:  # edge
        del = dist(cycle, i, i_left) + dist(cycle, j, j_right)
        add = dist(cycle, i, j_right) + dist(cycle, i_left, j)
    else:  # vertex
        if i == 0 and j == n - 1:  # extreme case
            del = dist(i, i_right) + dist(j, j_left)
            add = dist(i, j_left) + dist(j, i_right)
        elif j - i == 1:
            del = dist(i, i_left) + dist(j, j_right)
            add = dist(i, j_right) + dist(i_left, j)
        else:
            del = dist(i, i_left) + dist(i, i_right) + dist(j, j_left) + dist(j, j_right)
            add = dist(i, j_left) + dist(i, j_right) + dist(j, i_left) + dist(j, i_right)

    return del - add
```

### inter-class
```rust
fun get_objective_value(movement):
    i, j, n = movement[0], movement[1], cycle.size()
    del = dist(i, i_left) + dist(i, i_right) + dist(j, j_left) + dist(j, j_right)
    add = dist(i, j_left) + dist(i, j_right) + dist(j, i_left) + dist(j, i_right)
    return del - add
```

## Update cycles / apply movement
The vertex swap operation involves a simple swap of values in the given cycles/cycle at positions ``i`` and ``j``. The edge swap operation inverts all elements of the cycle (vector) located between (including) vertices with given indices.
### inner-class
```rust
fun apply_movement(movement):
    i, j, type = movement[0], movement[1], movement[2]
    if type == 0: 
        reverse(cycle[i], cycle[j] + 1) // edge
    else: 
        swap(cycle[i], cycle[j]) // vertex
```
### inter-class
```rust
fun apply_movement(movement):
    i, j = movement[0], movement[1]
    swap(cycle1[i], cycle2[j])
```

## Algorithm
```rust
fun local_search(cycle, steepest: bool):
    if inner: movements = <generate_edge_movements(), generate_vertex_movements()>
    else: movements = generate_vertex_movements_inter()
    do{
        found_better = false
        best_delta = 0
        movements.shuffle()
        for movement in movements:
            delta = gen_objective_value(movement)
            if delta > 0:
                found_better = true
                if steepest:
                    if delta > best_delta:
                        best_delta = delta
                        best_movement = movement
                else:
                    apply_movement(movement)
                    break;
        if steepest and found_better:
            apply_movement(best_movement)
    } while (found_better); 
```
The algorithm, depending on whether it allows inter-class or intra-class moves, defines its neighborhood. Then the while loop, will be executed until the algorithm manages to find a move that would improve the current solution - found_better. Inside the while, we iterate through all possible moves in a random order and using the delta of the objective function appropriate for the algorithm variant - we apply the move if the greedy method is called, or update the variable defining the best possible move if we are dealing with the steep version.

## Random walk
As a point of reference, a random stray algorithm was also implemented, which in each iteration performs a random move (regardless of its evaluation) and returns the best solution found in this way. This algorithm runs in the same amount of time as, on average, the slowest version of the local search on a given instance. For intra-class and inter-class operations, we have created two versions of this algorithm, which differ only in the set of moves.

## Results
The results presented, were obtained by executing each algorithm 100 times for each instance. Initial solutions were generated using the "Biggest 2-regret" heuristics and a random starting vertex was selected each time. The best, worst and average results for path lengths and execution times are shown in the tables below. <br> 

### Cycle lengths
| Algo                | Instance |  Min  |  Max  | Mean  |
|---------------------|----------|-------|-------|-------|
| Random-inner-greedy | kroA100  | 31202 | 38337 | 34573 |
|                     | kroB100  | 32325 | 38254 | 34969 |
| Random-inner-steepest | kroA100 | 31763 | 37201 | 33851 |
|                         | kroB100 | 31964 | 37370 | 34408 |
| Random-inter-greedy | kroA100  | 41811 | 74923 | 52136 |
|                     | kroB100  | 42124 | 85923 | 52937 |
| Random-inter-steepest | kroA100 | 39935 | 69595 | 51889 |
|                         | kroB100 | 41465 | 69670 | 52805 |
| Regret-inner-greedy | kroA100  | 21850 | 31795 | 26645 |
|                     | kroB100  | 25697 | 31609 | 28420 |
| Regret-inner-steepest | kroA100 | 21850 | 30411 | 26253 |
|                         | kroB100 | 25697 | 31609 | 28347 |
| Regret-inter-greedy | kroA100  | 21988 | 30567 | 25503 |
|                     | kroB100  | 25184 | 30764 | 27446 |
| Regret-inter-steepest | kroA100 | 21988 | 30491 | 25580 |
|                         | kroB100 | 25158 | 29837 | 27002 |
| Random-walk-inner | kroA100 | 154029 | 194096 | 174670
|                   | kroB100 | 151859 | 189902 | 173445
| Random-walk-inter | kroA100 | 151781 | 195356 | 171037
|                    | kroB100 | 149269 | 185413 | 168675
| Regret-walk-inner | kroA100 | 38715 |192824 |136851
|                    | kroB100 | 45050 |193364|124853
| Regret-walk-inter  | kroA100 | 21988 | 30575 |26563
|                     | kroB100 | 25968 | 32637 |28802



### Computation times
Unit: $\mu s$
| Algo                              | Instance |  Min   |  Max   | Mean |
|-----------------------------------|----------|-------|-------|-------|
| Random-inner-greedy               | kroA100  | 24522 | 34249 | 29895 |
|                                   | kroB100  | 25269 | 37114 | 30302 |
| Random-inner-steepest             | kroA100  | 28299 | 43927 | 32009 |
|                                   | kroB100  | 25862 | 38675 | 31515 |
| Random-inter-greedy               | kroA100  | 13012 | 41220 | 26128 |
|                                   | kroB100  | 12849 | 34434 | 24947 |
| Random-inter-steepest             | kroA100  | 17796 | 48287 | 30477 |
|                                   | kroB100  | 16777 | 44395 | 29631 |
| Regret-inner-greedy               | kroA100  | 1921  | 4396  | 2460  |
|                                   | kroB100  | 1950  | 4293  | 2613  |
| Regret-inner-steepest             | kroA100  | 1965  | 6648  | 2876  |
|                                   | kroB100  | 1934  | 5616  | 3045  |
| Regret-inter-greedy               | kroA100  | 779   | 5102  | 1974  |
|                                   | kroB100  | 1052  | 4857  | 2446  |
| Regret-inter-steepest             | kroA100  | 780   | 8623  | 3302  |
|                                   | kroB100  | 1485  | 7541  | 3641  |


## Best cycles [kroA100.tsp | kroB100.tsp]

Random-inner-greedy: <br>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/361110f7-bafa-4c43-ae31-85af1a5bfef2" alt="local_random_inner_greedy_kroA100" width="430"/>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/40f40d9a-7abf-40fe-91f0-1597825d3904" alt="local_random_inner_greedy_kroB100" width="430"/>

Random-inner-steepest: <br>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/d47a4170-23c9-4310-ae4d-828a46137f81" alt="local_random_inner_steepest_kroA100" width="430"/>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/0a8312cf-dc6c-42d2-a291-e9fef80f3ff0" alt="local_random_inner_steepest_kroB100" width="430"/>

Random-inter-greedy: <br>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/b4e1221d-a27c-4dda-b46a-100783037d8f" alt="local_random_inter_greedy_kroA100" width="430"/>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/54a81566-fd28-47b9-be25-4335216c7fc2" alt="local_random_inter_greedy_kroB100" width="430"/>

Random-inter-steepest: <br>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/0da594e3-7597-4776-a047-3c85edf771e6" alt="local_random_inter_steepest_kroA100" width="430"/>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/4f36b2ef-d0dd-45c3-aa36-9187645ce918" alt="local_random_inter_steepest_kroB100" width="430"/>

Regret-inner-greedy: <br>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/af0df682-5c3a-487d-a1bf-734e176c1275" alt="local_regret_inner_greedy_kroA100" width="430"/>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/60f538bc-e424-4eda-878e-bc6bf957a585" alt="local_regret_inner_greedy_kroB100" width="430"/>

Regret-inner-steepest: <br>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/6b44e353-75a3-49fd-9bd2-665f0adab92e" alt="local_regret_inner_steepest_kroA100" width="430"/>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/a5588b32-1acd-405b-810c-8cd997fb6ffe" alt="local_regret_inner_steepest_kroB100" width="430"/>

Regret-inter-greedy: <br>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/ebdc2d54-014e-4e63-88e2-e89f80a8829c" alt="local_regret_inter_greedy_kroA100" width="430"/>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/5d51d333-2fbc-4027-8389-05d5591cba1c" alt="local_regret_inter_greedy_kroB100" width="430"/>

Regret-inter-steepest: <br>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/0dc3df8b-9c73-4330-bc7e-f810ba27dbe5" alt="local_regret_inter_steepest_kroA100" width="430"/>
<img src="https://github.com/kamil271e/double-tsp/assets/82380348/0eba46db-a1f6-4a8f-982d-ea8aa995df0d" alt="local_regret_inter_steepest_kroB100" width="430"/>


## Conclusions
According to the assumptions, all algorithm variants obtain better results than random wandering. It is also natural that the cycle lengths obtained after starting from a 2-regret-based solution are significantly smaller than those that started with random cycles. The same is true for execution times - algorithms that start their search from a ready solution find the local optimum faster.
