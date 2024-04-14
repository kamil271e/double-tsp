#include <set>
#include <tuple>
#include "../lib/tsp.h"

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
        search_memory();
    }
    
    return {cycle1, cycle2};

}

auto TSP::init_LM(std::vector<std::vector<int>> movements) -> std::multiset<TupleType, TupleComparator>{
    std::multiset<TupleType, TupleComparator> LM;
    for (auto& movement : movements) {
        int delta, cycle_num;
        std::tie(delta, cycle_num) = get_delta(movement);
        if (delta > 0) {
            LM.insert({movement[0], movement[1], movement[3], cycle_num, 1, delta});
        }
    }
    return LM;
}


auto TSP::search_memory() -> std::tuple <std::vector<int>, std::vector<int>> {
    auto adj_matrix1 = generate_adj_matrix(cycle1);
    auto adj_matrix2 = generate_adj_matrix(cycle2);
    std::vector<std::vector<int>> movements;
//    std::vector<std::vector<int>> movements_inter = generate_all_vertex_movements_inter(cycle1.size());
//    movements.insert(movements.end(), movements_inter.begin(), movements_inter.end());
    std::vector<std::vector<int>> movements_inner = generate_all_edge_movements(cycle1.size());
    movements.insert(movements.end(), movements_inner.begin(), movements_inner.end());

    // ALGORITHM
    // add all movements that  delta > 0  to LM
    // repeat
        // check all new moves based on previous move
        // for m in LM
            // if edge not in cycle - delete m
            // if edge1 edge2 reversed - continue
            // if applicable - > apply
            // remember (i, j) for next iteration
    // until no move was applied after traversing entire LM

    int start_len = calc_cycles_len();
    int n = cycle1.size();
    bool found_better;
    std::multiset<TupleType, TupleComparator> LM = init_LM(movements);
    do{
        found_better = false;
        for (auto it = LM.begin(); it != LM.end();) {
            auto move = *it;
            int i = std::get<0>(move);
            int j = std::get<1>(move);
            int edge_or_vertex = std::get<2>(move);
            int cycle_num = std::get<3>(move);
            int applicable = std::get<4>(move);
            int delta = std::get<5>(move);
            std::vector<int>& cycle = (cycle_num == 0) ? cycle1 : cycle2;

            int edge11 = cycle[(i - 1 + n) % n];
            int edge12 = cycle[i];

            int edge21 = cycle[j];
            int edge22 = cycle[(j + 1) % n];

            if (adj_matrix1[edge11][edge12] == 0 || adj_matrix1[edge21][edge22] == 0) {
                it = LM.erase(it);
                continue;
            }
            if (adj_matrix1[edge11][edge12] == -1 && adj_matrix1[edge21][edge22] == -1) {
                ++it;
                continue;
            }
            apply_movement({i, j, 0, 0, cycle_num});
            found_better = true;
            ++it;
        }
    }while(found_better);

    int end_len = calc_cycles_len();
//    for (auto& t : LM) {
//        std::cout << std::get<0>(t) << " " << std::get<1>(t) << " " << std::get<2>(t) << " " << std::get<3>(t) << " " << std::get<4>(t) << " " << std::get<5>(t) << std::endl;
//    }

    std::cout << start_len << " " << end_len << std::endl;
    return {cycle1, cycle2};
}

auto TSP::generate_adj_matrix(const std::vector<int>& cycle) -> std::vector<std::vector<int>>{
    int n = cycle.size();
    std::vector<std::vector<int>> adj_matrix(2 * n, std::vector<int>(2 * n, 0));
    for (int i = 0; i < n; i++){
        adj_matrix[cycle[i]][cycle[(i+1)%n]] = 1;
        adj_matrix[cycle[(i+1)%n]][cycle[i]] = -1;
    }
    return adj_matrix;
}

void TSP::visualize_adj_matrix(std::vector<std::vector<int>> adj_matrix){
    for (int i = 0; i < adj_matrix.size(); ++i){
        for (int j = 0; j < adj_matrix.size(); ++j){
            std::cout << adj_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

auto TSP::search_candidates() -> std::tuple<std::vector<int>, std::vector<int>>{

    std::vector<int> movement;
    std::vector<int> best_movement = {};
    int best_objective_value = 0;
    int objective_value;
    int cycle_num;

    bool found_better  = false;
    int k = 10;


    for (int a = 0; a < dist_matrix.x_coord.size(); ++a) {
        auto nearest_vertices = find_nearest_vertices(a, k);
        for (int b : nearest_vertices) {
            // Find cycles c1, c2 and indices i, j of nodes a and b 
            auto [c1, i] = find_node({cycle1, cycle2}, a);
            auto [c2, j] = find_node({cycle1, cycle2}, b);
            objective_value = 0;
            

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
                found_better = true;
            }
        }
    }

    // If we found a better movement, we apply it
    if (found_better) {
        apply_movement(best_movement);
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


