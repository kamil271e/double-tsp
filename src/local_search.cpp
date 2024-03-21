#include "../lib/tsp.h"

// Function to replacement of two vertices included in the path
auto TSP::generate_neighbors(const std::vector<int>& x, int n) -> std::vector<std::vector<int>>{

    std::vector<std::vector<int>> neighbors;
    std::set<std::vector<int>> unique_neighbors;
    std::random_device rd;
    std::mt19937 gen(rd());

    while (neighbors.size() < n) {
        std::uniform_int_distribution<> dis_i(0, x.size() - 2);
        std::uniform_int_distribution<> dis_j(1, x.size() - 1);

        int i = dis_i(gen);
        int j = dis_j(gen);

        if (i > j)
            std::swap(i, j);

        std::vector<int> neighbor = x;
        std::reverse(neighbor.begin() + i, neighbor.begin() + j);

        if (unique_neighbors.find(neighbor) == unique_neighbors.end()) {
            neighbors.push_back(neighbor);
            unique_neighbors.insert(neighbor);
        }
    }

    // Return the vector of neighbors
    return neighbors;
}


// Function to generate all possible neighbors by swapping each edge replacement
auto TSP::generate_all_edge_movements(const std::vector<int>& x) -> std::vector<std::vector<int>> {
    // edge movement : type = 0
    // vertx movement: type = 1
    std::vector<std::vector<int>> neighbors; // i, j, type
    int n = x.size();

    // Generate all possible neighbors by swapping each pair of vertices
    for (int i = 0; i < n; ++i) {
        for (int j = i + 2; j < n; ++j) {
            if (i==0 && j==n-1) continue;
            neighbors.push_back({i, j, 0});
        }
    }

    return neighbors;
}

auto TSP::generate_all_vertex_movements(const std::vector<int>& x) -> std::vector<std::vector<int>> {
    std::set<std::vector<int>> unique_neighbors;
    std::vector<std::vector<int>> neighbors; // i,j,id

    int n = x.size();

    // Generate all possible neighbors by swapping each pair of vertices
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            std::vector<int> neighbor = x;
            std::swap(neighbor[i], neighbor[j]);
            if (unique_neighbors.find(neighbor) == unique_neighbors.end()) {
                neighbors.push_back(neighbor);
                unique_neighbors.insert(neighbor);
            }

        }
    }

    return neighbors;
}

// Function to calculate the delta of the path / objective value
float TSP::get_objective_value(const std::vector<int>& cycle, std::vector<int> neighbor) {
    int i0 = neighbor[0];
    int j0 = neighbor[1];
    int n = cycle.size();
//    int i1 = i0 - 1; // c++ not support negative indices! : -1
//    int j1 = j0 + 1;
//    if (i1 < 0) i1 = n - 1;
//    if (j1 > n - 1) j1 = 0;
    int i1 = (i0 - 1 + n) % n;
    int j1 = (j0 + 1) % n;

    if (neighbor[2] == 0){ // edge
        // i = 2
        // j = 5
        // 1 2 [3] 4 5 [6] 7
        // 1 2 [6] 5 4 [3] 7
        // new 3,7 i 2,6:     i, (j+1)%size --- (i-1)%size, j
        // delete 2,3 i 6,7: (i-1)%size, i  --- (j+1)%size, j
        float deleted = dist_matrix.dist_matrix[cycle[i0]][cycle[i1]] + dist_matrix.dist_matrix[cycle[j0]][cycle[j1]];
        float added = dist_matrix.dist_matrix[cycle[i0]][cycle[j1]] + dist_matrix.dist_matrix[cycle[i1]][cycle[j0]];
        float delta = deleted - added;
        std::cout << "delta: " << delta << std::endl;
        return delta;
    } else{ // vertex
        // TODO
    }
    return 0;
}

auto TSP::find_random_neighbor(std::vector<std::vector<int>> neighbors) -> std::vector<int> {
    // Choose a random neighbor
    return neighbors[std::rand() % neighbors.size()];
}

auto TSP::hill_climbing(const std::vector<int>& init_cycle, bool steepest = false) -> std::vector<int>
{
    // Generate neighbors
    std::vector<std::vector<int>> egde_movements;
    egde_movements = generate_all_edge_movements(init_cycle);

    std::vector<std::vector<int>> neighbors;
    neighbors.insert(neighbors.end(), egde_movements.begin(), egde_movements.end());

    //std::vector<std::vector<int>> vertex_movements;
    //vertex_movements = generate_all_vertex_movements(init_cycle);
    //neighbors.insert(neighbors.end(), vertex_movements.begin(), vertex_movements.end());

    std::vector<int> cycle = init_cycle;
    std::cout << "CYCLE LENGTH BEFORE: " << calc_cycle_len(cycle) << std::endl;

    for (int iter = 0; iter < neighbors.size(); ++iter) { // for now typical iteration, later we need to chose them randomly
        if (get_objective_value(cycle, neighbors[iter]) > 0) { // better than current
            int i = neighbors[iter][0];
            int j = neighbors[iter][1];
            int type = neighbors[iter][2];
            if (steepest) {
                // modify cycle and continue
            } else {
                if (type == 0){ // edge
//                    std::cout << "REVERSE" << std::endl;
                    std::reverse(cycle.begin() + i, cycle.begin() + j + 1);
                    break;
                }else{// vertex

                }
                break;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "CYCLE LENGTH AFTER: " << calc_cycle_len(cycle) << std::endl;
    return cycle;

};

// Function to perform local search
auto TSP::local_search() -> std::tuple<std::vector<int>, std::vector<int>>
{
    // TODO: we should be able to choose starting cycles
    // auto [cycle1, cycle2] = find_greedy_cycles();
    auto [cycle1, cycle2] = generate_random_cycles(100);

    std::vector<int> hill_cycle1;
    std::vector<int> hill_cycle2;

    std::cout << "INITIAL CYCLES: " << std::endl;
    std::cout << "Cycle 1: ";
    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
//    std::cout << "Cycle 2: ";
//    for (int vertex : cycle2) {
//        std::cout << vertex + 1 << " ";
//    }
//    std::cout << std::endl;


    std::cout << "START LOCAL SEARCH" << std::endl;
    hill_cycle1 = hill_climbing(cycle1, false);
//    hill_cycle2 = hill_climbing(cycle2, false);

    return std::make_tuple(hill_cycle1, hill_cycle2);

};

auto TSP::generate_random_cycles(int n) -> std::tuple<std::vector<int>, std::vector<int>>
{
    std::vector<int> values(n);
    for (int i = 0; i < n; ++i) values[i] = i;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(values.begin(), values.end(), g);

    std::vector<int> cycle1(values.begin(), values.begin() + (int)(n/2));
    std::vector<int> cycle2(values.begin() + n - (int)(n/2), values.end());

    return {cycle1, cycle2};
};