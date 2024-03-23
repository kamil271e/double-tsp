#include "../lib/tsp.h"


auto TSP::generate_all_edge_movements(int n) -> std::vector<std::vector<int>> {
    // edge movement : type = 0
    // vertx movement: type = 1
    std::vector<std::vector<int>> movements; // i, j, type
    for (int i = 0; i < n; ++i) {
        for (int j = i + 2; j < n; ++j) {
            if (i==0 && j==n-1) continue;
            movements.push_back({i, j, 0});
        }
    }
    return movements;
}


auto TSP::generate_all_vertex_movements(int n) -> std::vector<std::vector<int>> {
    // edge movement : type = 0
    // vertx movement: type = 1
    std::vector<std::vector<int>> movements; // i, j, type
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            movements.push_back({i, j, 1});
        }
    }
    return movements;
}


auto TSP::generate_all_vertex_movements_inter(int n) -> std::vector<std::vector<int>> {
    // vertx movement: type = 1
    std::vector<std::vector<int>> movements; // i, j, type
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
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
        deleted = dist_matrix.dist_matrix[cycle[i]][cycle[i_left]] + dist_matrix.dist_matrix[cycle[j]][cycle[j_right]];
        added = dist_matrix.dist_matrix[cycle[i]][cycle[j_right]] + dist_matrix.dist_matrix[cycle[i_left]][cycle[j]];
    } else{ // vertex
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


void TSP::inner_class_search(std::vector<int>& cycle, bool steepest = false)
{
    std::vector<std::vector<int>> movements;
    std::vector<std::vector<int>> edge_movements = generate_all_edge_movements(cycle.size());
    std::vector<std::vector<int>> vertex_movements = generate_all_vertex_movements(cycle.size());
    movements.insert(movements.end(), edge_movements.begin(), edge_movements.end());
    movements.insert(movements.end(), vertex_movements.begin(), vertex_movements.end());
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


void TSP::inter_class_search(bool steepest) {
    std::vector<std::vector<int>> movements = generate_all_vertex_movements_inter(cycle1.size()); // only vertex movements for inter class
    // interpretation of movements vector: pair <indx_of_el_in_cycle1, indx_of_el_in_cycle2>

    std::map<int, int> cycle_num; // vertex_num: corresponding_cycle
    for (int i = 0; i < cycle1.size(); ++i) {
        cycle_num[cycle1[i]] = 1;
    }
    for (int i = 0; i < cycle2.size(); ++i) {
        cycle_num[cycle2[i]] = 2;
    }

    std::vector<std::vector<int>> visited_movements;
    std::vector<int> best_movement;
    bool found_better = false;
    int best_objective_value = 0;

    // this logic is not ideal yet - need to work with this visited_movements (not sure if they are necessary)
    do{
        std::shuffle(movements.begin(), movements.end(), std::mt19937(std::random_device()())); // shuffle movements
        for (int iter = 0; iter < movements.size(); ++iter) {
            // if (invalid_move_check(cycle_num, movements[iter])) continue;
            found_better = false;
            float objective_value = get_objective_value(movements[iter]);
            if (objective_value > 0 && std::find(visited_movements.begin(), visited_movements.end(), movements[iter]) == visited_movements.end()) { // better than current
                visited_movements.push_back(movements[iter]);
                found_better = true;
                if (steepest) {
                    if (objective_value > best_objective_value) {
                        best_objective_value = objective_value;
                        best_movement = movements[iter];
                    }
                } else {
                    update_cycles(movements[iter], cycle_num);
                    break;
                }
            }
        }
        if (steepest && found_better){
            update_cycles(best_movement, cycle_num);
        }
    } while (found_better);
}


void TSP::update_cycles(std::vector<int> &movement, std::map<int, int> &cycle_num) {
    int i = movement[0]; // idx of vertex in cycle1
    int j = movement[1]; // idx of vertex in cycle2
    int temp = cycle1[i];
    cycle1[i] = cycle2[j];
    cycle2[j] = temp;
    cycle_num[cycle1[i]] = 1;
    cycle_num[cycle2[j]] = 2;
}


bool TSP::invalid_move_check(std::map<int, int> cycle_num, std::vector<int> movement) {
    int i = movement[0];
    int j = movement[1];
    if (cycle_num[i] != cycle_num[j]) return false;
    return true;
}


float TSP::get_objective_value(std::vector<int> movement) {
    int i = movement[0];
    int j = movement[1];
    int n = cycle1.size();
    int i_left = (i - 1 + n) % n;
    int i_right = (i + 1) % n;
    int j_left = (j - 1 + n) % n;
    int j_right = (j + 1) % n;
    float deleted, added;

    deleted = dist_matrix.dist_matrix[cycle1[i]][cycle1[i_left]] + dist_matrix.dist_matrix[cycle1[i]][cycle1[i_right]]
              + dist_matrix.dist_matrix[cycle2[j]][cycle2[j_left]] + dist_matrix.dist_matrix[cycle2[j]][cycle2[j_right]];
    added = dist_matrix.dist_matrix[cycle1[i]][cycle2[j_left]] + dist_matrix.dist_matrix[cycle1[i]][cycle2[j_right]]
            + dist_matrix.dist_matrix[cycle2[j]][cycle1[i_left]] + dist_matrix.dist_matrix[cycle2[j]][cycle1[i_right]];

    return deleted - added;
}


// Function to perform local search
auto TSP::local_search() -> std::tuple<std::vector<int>, std::vector<int>>
{
    // BE CAREFUL! -- greedy regret is deterministic since by default it starts with vertex no. 1
    // we should probably change that to consider all possible starts ?
    // auto [rand1, rand2] = generate_random_cycles(100);

    auto [regret1, regret2] = find_greedy_cycles_regret();
    cycle1 = regret1;
    cycle2 = regret2;

    std::cout << "INITIAL CYCLES: " << std::endl;
    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
    for (int vertex : cycle2) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
    std::cout << calc_cycles_len() << std::endl;


    std::cout << "START LOCAL SEARCH" << std::endl;
    // INNER CLASS SEARCH
    //    inner_class_search(cycle1, false);
    //    inner_class_search(cycle2, false);

    // INTER CLASS SEARCH
     inter_class_search(false);

    std::cout << calc_cycles_len() << std::endl;

    return {cycle1, cycle2};

}
