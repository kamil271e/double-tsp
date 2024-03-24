# Local search

## Utils methods
### Generate edge movements
```rust
fun generate_edge_movements():
    movements = vector<<idx_i, idx_j, type>>
    for i = 0 to total_vertices():
        for j = i + 2 to total_vertices():
            if (i==0 && j==n-1) continue;
            movements.push_back({i, j, 0})
    return movements
```
### Generate vertex movements
```rust
fun generate_vertex_movements():
    movements = vector<<idx_i, idx_j, type>>
    for i = 0 to total_vertices():
        for j = 0 to total_vertices():
            if (i==j) continue;
            movements.push_back({i, j, 1})
    return movements
```
### Generate vertex movements - inter-class
```rust
fun generate_vertex_movements_inter():
    movements = vector<<idx_i, idx_j, type>>
    for i = 0 to total_vertices():
        for j = 0 to total_vertices():
            movements.push_back({i, j, 0})
    return movements
```

### Get objective value (delta) - inner-class
Variables interpretation:
```rust
i_left, i_right = (i - 1 + n) % n, (i + 1) % n;
j_left, j_right = (j - 1 + n) % n, (j + 1) % n;
fun dist(i, j): return M[cycle[i]][cycle[j]]
```

``M``: distance matrix <br>
``i, i_left, i_right``: cycle1 <br>
``j, j_left, j_right``: cycle2 <br>
```rust
fun gen_objective_value(movement):
    i, j, n = movement[0], movement[1], cycle.size()
    del = dist(i, i_left) + dist(i, i_right) + dist(j, j_left) + dist(j, j_right)
    add = dist(i, j_left) + dist(i, j_right) + dist(j, i_left) + dist(j, i_right)
    return del - add
```

### Get objective value (delta) - inter-class
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


## Algorithms

## Results

## Best cycles [kroA100.tsp | kroB100.tsp]

