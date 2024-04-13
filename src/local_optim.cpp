# include "../lib/tsp.h"



auto TSP::local_optim() -> std::tuple<std::vector<int>, std::vector<int>>
{
    
    // Convert to cycles

    local_search();
    std::cout << "After local search" << std::endl;
    //Print cycles
    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
    for (int vertex : cycle2) {
        std::cout << vertex + 1 << " ";
    }

    search_candidates();
    std::cout << "\nAfter search candidates" << std::endl;
    return {cycle1, cycle2};

}

auto TSP::search_candidates() -> std::tuple<std::vector<int>, std::vector<int>>{

    std::vector<int> movement;
    std::vector<int> best_movement = {};
    int best_objective_value = 0;
    int objective_value;
    int cycle_num, best_cycle_num;

    bool found_better  = false;
    int k = 10;


    for (int a = 0; a < dist_matrix.x_coord.size(); ++a) {
        auto nearest_vertices = find_nearest_vertices(a, k);
        for (int b : nearest_vertices) {
            // Find cycles c1, c2 and indices i, j of nodes a and b 
            auto [c1, i] = find_node({cycle1, cycle2}, a);
            auto [c2, j] = find_node({cycle1, cycle2}, b);
            movement, objective_value = {}, 0;
            
            //std::cout << "c1: " << c1 << " c2: " << c2 << " i: " << i << " j: " << j << std::endl;

            if (c1 == c2) {
                // move := swap inner edges (a, succ(a)) and (b, succ(b))
                movement = {i, j, 0, 0};
            } else {
                // move := swap nodes a and b
                movement = {i, j, 1, 1};
            }

            
            std::tie(objective_value, cycle_num) = get_delta(movement);
            //std::cout << "Objective value: " << objective_value << std::endl;

            if (objective_value < best_objective_value) {
                best_objective_value = objective_value;
                best_movement = movement;
                best_cycle_num = cycle_num;
                found_better = true;
            }
        }
    }

    if (found_better) {
        std::cout << "\n\nFinded better movement:" << std::endl;
        std::cout << "Best movement: " << best_movement[0] << " " << best_movement[1] << " " << best_movement[2] << " " << best_movement[3] << " " << std::endl;
        std::cout << "Cycle num: " << best_cycle_num << std::endl;
        apply_movement(best_movement, best_cycle_num);
    }

 

    return {cycle1, cycle2};

}

auto TSP::find_nearest_vertices(int vertex, int k) -> std::vector<int>{
    std::vector<int> nearest_vertices;
    std::vector<std::pair<int, double>> distances;

    // Get the distances from the vertex to all other vertices
    for (int i = 0; i < dist_matrix.x_coord.size(); ++i) {
        if (i != vertex) {
            distances.push_back({i, dist_matrix.dist_matrix[vertex][i]});
        }
    }

    // Sort the distances
    std::sort(distances.begin(), distances.end(), [](auto& left, auto& right) {
        return left.second < right.second;
    });

    // Get the k nearest vertices
    for (int i = 0; i < k; ++i) {
        nearest_vertices.push_back(distances[i].first);
    }


    return nearest_vertices;    

}

auto TSP::find_node(std::tuple<std::vector<int>, std::vector<int>> cycles, int node) -> std::pair<int, int>{ 


    std::vector<int>& cycle1 = std::get<0>(cycles);
    auto it1 = std::find(cycle1.begin(), cycle1.end(), node);

    // If element was found 
    if (it1 != cycle1.end())  
    { 
        // calculating the index 
        int index = it1 - cycle1.begin(); 
        return {0, index};
    }

    std::vector<int>& cycle2 = std::get<1>(cycles);
    auto it2 = std::find(cycle2.begin(), cycle2.end(), node); 

    if (it2 != cycle2.end())  
    { 
        // calculating the index 
        int index = it2 - cycle2.begin(); 
        return {1, index};
    }


    assert(false && "City must be in either cycle");   
}
