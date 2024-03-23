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
    return deleted - added;
}


auto TSP::hill_climbing(const std::vector<int>& init_cycle, bool steepest = false) -> std::vector<int>
{
    std::vector<std::vector<int>> movements;
    std::vector<std::vector<int>> edge_movements = generate_all_edge_movements(init_cycle);
    std::vector<std::vector<int>> vertex_movements = generate_all_vertex_movements(init_cycle);
    movements.insert(movements.end(), edge_movements.begin(), edge_movements.end());
    movements.insert(movements.end(), vertex_movements.begin(), vertex_movements.end());
    std::vector<int> cycle = init_cycle;
    std::vector<std::vector<int>> visited_movements;
    std::vector<int> best_movement;
    bool found_better = false;
    int best_objective_value = 0;

    // this logic is not ideal yet - need to work with this visited_movements (not sure if they are necessary)
    do{
        std::shuffle(movements.begin(), movements.end(), std::mt19937(std::random_device()())); // shuffle movements
        for (int iter = 0; iter < movements.size(); ++iter) {
            found_better = false;
            float objective_value = get_objective_value(cycle, movements[iter]);
            if (objective_value > 0 && std::find(visited_movements.begin(), visited_movements.end(), movements[iter]) == visited_movements.end()) { // better than current
                visited_movements.push_back(movements[iter]);
                found_better = true;
                if (steepest) {
                    if (objective_value > best_objective_value) {
                        best_objective_value = objective_value;
                        best_movement = movements[iter];
                    }
                } else {
                    update_cycle(movements[iter], cycle);
                    break;
                }
            }
        }
        if (steepest && found_better){
            update_cycle(best_movement, cycle);
        }
    } while (found_better);
    return cycle;
}

void TSP::update_cycle(const std::vector<int>& movement, std::vector<int>& cycle) {
    int i = movement[0];
    int j = movement[1];
    int type = movement[2];
    if (type == 0){ // edge
        std::reverse(cycle.begin() + i, cycle.begin() + j + 1);
    }else{ // vertex
        std::swap(cycle[i], cycle[j]);
    }
}

// Function to perform local search
auto TSP::local_search() -> std::tuple<std::vector<int>, std::vector<int>>
{
    // TODO: we should be able to choose starting cycles
    // BE CAREFUL! -- greedy regret is deterministic since by default it starts with vertex no. 1
    // we should probably change that to consider all possible starts ?
    auto [cycle1, cycle2] = find_greedy_cycles_regret();
    // auto [cycle1, cycle2] = generate_random_cycles(100);
    std::vector<int> hill_cycle1;
    std::vector<int> hill_cycle2;

    std::cout << "INITIAL CYCLES: " << std::endl;
    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
    for (int vertex : cycle2) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;


    std::cout << "START LOCAL SEARCH" << std::endl;
    hill_cycle1 = hill_climbing(cycle1, false);
    hill_cycle2 = hill_climbing(cycle2, false);

    return {hill_cycle1, hill_cycle2};

}
