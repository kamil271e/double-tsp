#include "../lib/tsp.h"


auto TSP::generate_all_edge_movements(const std::vector<int>& cycle) -> std::vector<std::vector<int>> {
    // edge movement : type = 0
    // vertx movement: type = 1
    std::vector<std::vector<int>> movements; // i, j, type
    int n = cycle.size();

    for (int i = 0; i < n; ++i) {
        for (int j = i + 2; j < n; ++j) {
            if (i==0 && j==n-1) continue;
            movements.push_back({i, j, 0});
        }
    }
    return movements;
}

auto TSP::generate_all_vertex_movements(const std::vector<int>& cycle) -> std::vector<std::vector<int>> {
    // edge movement : type = 0
    // vertx movement: type = 1
    std::vector<std::vector<int>> movements; // i, j, type
    int n = cycle.size();

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            movements.push_back({i, j, 1});
        }
    }
    return movements;
}

// Function to calculate the delta of the path / objective value
float TSP::get_objective_value(const std::vector<int>& cycle, std::vector<int> movement) {
    int i = movement[0];
    int j = movement[1];
    int n = cycle.size();
    int i_left = (i - 1 + n) % n;
    int i_right = (i + 1) % n;
    int j_left = (j - 1 + n) % n;
    int j_right = (j + 1) % n;
    float deleted, added;
    
    if (movement[2] == 0){ // edge
        /*
         * i = 2, j = 5
         * 1 2 [3] 4 5 [6] 7
         * 1 2 [6] 5 4 [3] 7
         * new 3,7 i 2,6:     (i, j+1), (i-1, j)
         * delete 2,3 i 6,7:  (i-1, i)  (j+1, j)
         * basic logic - delete this comment if understand
         * */
        deleted = dist_matrix.dist_matrix[cycle[i]][cycle[i_left]] + dist_matrix.dist_matrix[cycle[j]][cycle[j_right]];
        added = dist_matrix.dist_matrix[cycle[i]][cycle[j_right]] + dist_matrix.dist_matrix[cycle[i_left]][cycle[j]];
        // std::cout << "DELTA: " << deleted - added << std::endl;
    } else{ // vertex
        /*
         * delete: (i, i-1), (i,i+1), (j, j-1), (j, j+1)
         * add: (i, j-1), (i, j+1), (j, i-1), (j, i+1)
         * basic logic - delete this comment if understand
         * */
        if (i == j - 1 || j == i - 1 || (i == 0 && j == n-1) || (j == 0 && i == n-1) ){
            deleted = dist_matrix.dist_matrix[cycle[i]][cycle[i_left]] + dist_matrix.dist_matrix[cycle[j]][cycle[j_right]];
            added = dist_matrix.dist_matrix[cycle[i]][cycle[j_right]] + dist_matrix.dist_matrix[cycle[j]][cycle[i_left]];
        }else{
            deleted = dist_matrix.dist_matrix[cycle[i]][cycle[i_left]] + dist_matrix.dist_matrix[cycle[i]][cycle[i_right]]
                      + dist_matrix.dist_matrix[cycle[j]][cycle[j_left]] + dist_matrix.dist_matrix[cycle[j]][cycle[j_right]];
            added = dist_matrix.dist_matrix[cycle[i]][cycle[j_left]] + dist_matrix.dist_matrix[cycle[i]][cycle[j_right]]
                    + dist_matrix.dist_matrix[cycle[j]][cycle[i_left]] + dist_matrix.dist_matrix[cycle[j]][cycle[i_right]];
        }
    }
    std::cout << deleted - added << std::endl;
    return deleted - added;
}

auto TSP::get_random_move(std::vector<std::vector<int>> movements) -> std::vector<int> {
    return movements[std::rand() % movements.size()];
}

auto TSP::hill_climbing(const std::vector<int>& init_cycle, bool steepest = false) -> std::vector<int>
{
    std::vector<std::vector<int>> movements;
//    std::vector<std::vector<int>> edge_movements = generate_all_edge_movements(init_cycle);
//    movements.insert(movements.end(), edge_movements.begin(), edge_movements.end());

    std::vector<std::vector<int>> vertex_movements = generate_all_vertex_movements(init_cycle);
    movements.insert(movements.end(), vertex_movements.begin(), vertex_movements.end());

//    std::shuffle(movements.begin(), movements.end(), std::mt19937(std::random_device()())

    std::vector<int> cycle = init_cycle;
    float before = calc_cycle_len(cycle);

    for (int iter = 0; iter < movements.size(); ++iter) { // for now regular iteration, later we need to chose them randomly
        if (get_objective_value(cycle, movements[iter]) > 0) { // better than current
            int i = movements[iter][0];
            int j = movements[iter][1];
            int type = movements[iter][2];
            if (steepest) {
                // modify cycle and continue
            } else {
                if (type == 0){ // edge
                    std::reverse(cycle.begin() + i, cycle.begin() + j + 1);
                }else{ // vertex
                    std::swap(cycle[i], cycle[j]);
                }
                break;
            }
        }
    }

    std::cout << std::endl;
    float after = calc_cycle_len(cycle);
    std::cout << "DELTA: " << after - before << std::endl;
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

    return {hill_cycle1, hill_cycle2};

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