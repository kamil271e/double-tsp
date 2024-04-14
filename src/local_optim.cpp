# include "../lib/tsp.h"


auto TSP::local_optim() -> std::tuple<std::vector<int>, std::vector<int>>
{
    
    //Generate input cycles
    std::tie(cycle1, cycle2) = generate_random_cycles(100);

    if (alg_type == AlgType::search_candidates) {
       //Measure time
        auto start_time_local = std::chrono::high_resolution_clock::now();
        search_candidates();
        auto end_time_local = std::chrono::high_resolution_clock::now();
        auto duration_local = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_local - start_time_local).count();
        //save_time(duration_local, params, "search_candidates"); //TODO: Implement save_time for search_candidates
    }
    else if(alg_type == AlgType::search_memory){
        ;
    }
    

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
            

            // If the nodes are in the same cycle, we swap the inner edges
            if (c1 == c2) {
                // move := swap inner edges 
                movement = {i, j, 0, 0};
            } else { // If the nodes are in different cycles, we swap the inter vertices
                // move := swap inter vertices 
                movement = {i, j, 1, 1};
            }

            // Get the objective value of the movement
            std::tie(objective_value, cycle_num) = get_delta(movement);

            // If the objective value is better than the current best, we update the best movement
            if (objective_value > best_objective_value) {
                best_objective_value = objective_value;
                best_movement = movement;
                best_cycle_num = cycle_num;
                found_better = true;
            }
        }
    }

    // If we found a better movement, we apply it
    if (found_better) {
        apply_movement(best_movement, best_cycle_num);
    }

    return {cycle1, cycle2};

}


// Find the k nearest vertices to a given vertex
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

// Find the cycle and index of a given node
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
