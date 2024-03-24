# Local search

## Generate movements / neighbourhood
Movements consist of tuples: ``(idx_i, idx_j, type)`` where ``idx_i`` and ``idx_j`` are indices of the cycle/cycles and ``type`` is the type of movement (0: edge, 1: vertex). <br>
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
fun gen_objective_value(movement):
    i, j, n = movement[0], movement[1], cycle.size()
    del = dist(i, i_left) + dist(i, i_right) + dist(j, j_left) + dist(j, j_right)
    add = dist(i, j_left) + dist(i, j_right) + dist(j, i_left) + dist(j, i_right)
    return del - add
```

### inter-class
```rust
fun gen_objective_value(movement):
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

## Update cycles / apply movement
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

## Results

## Best cycles [kroA100.tsp | kroB100.tsp]

