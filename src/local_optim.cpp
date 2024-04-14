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

auto TSP::init_LM(std::vector<std::vector<int>>& movements) -> std::multiset<TupleType, TupleComparator>{
    std::multiset<TupleType, TupleComparator> LM;
    std::vector<std::vector<int>> filtered_movements;
    for (auto& movement : movements) {
        int delta, cycle_num;
        std::tie(delta, cycle_num) = get_delta(movement);
        if (delta > 0) {
            LM.insert({movement[0], movement[1], movement[3], cycle_num, 1, delta});
        } else{
            filtered_movements.push_back(movement);
        }
    }
    movements = filtered_movements;
    return LM;
}

void TSP::update_LM(std::vector<std::vector<int>>& movements, std::multiset<TupleType, TupleComparator>& LM){
    std::vector<std::vector<int>> filtered_movements;
    for (auto& movement : movements) {
        int delta, cycle_num;
        std::tie(delta, cycle_num) = get_delta(movement);
        if (delta > 0) {
            LM.insert({movement[0], movement[1], movement[3], cycle_num, 1, delta});
        } else{
            filtered_movements.push_back(movement);
        }
    }
    movements = filtered_movements;
}


void TSP::update_adj_matrix(std::vector <std::vector<int>> & adj_matrix, std::vector<int> &cycle, int i, int j) {
    int n = cycle.size();
    int i_left = (i - 1 + n) % n;
    int j_right = (j + 1) % n;

    // we need to delete edges (i-1, i) (j, j+1) and add edges (i,j+1) (i-1,j)
    adj_matrix[cycle[i_left]][cycle[i]] = 0;
    adj_matrix[cycle[i]][cycle[i_left]] = 0;
    adj_matrix[cycle[j]][cycle[j_right]] = 0;
    adj_matrix[cycle[j_right]][cycle[j]] = 0;

    adj_matrix[cycle[i]][cycle[j_right]] = 1;
    adj_matrix[cycle[j_right]][cycle[i]] = -1;
    adj_matrix[cycle[i_left]][cycle[j]] = 1;
    adj_matrix[cycle[j]][cycle[i_left]] = -1;

    for (int k = i + 1; k < j - 1; ++k) {
        reverse_edges(adj_matrix, cycle, k, k + 1);
    }
}


void TSP::reverse_edges(std::vector<std::vector<int>> & adj_matrix, std::vector<int> &cycle, int i, int j) {
    adj_matrix[cycle[i]][cycle[j]] = -1;
    adj_matrix[cycle[j]][cycle[i]] = 1;
}


auto TSP::search_memory() -> std::tuple <std::vector<int>, std::vector<int>> {
    adj_matrix1 = generate_adj_matrix(cycle1);
    adj_matrix2 = generate_adj_matrix(cycle2);
    std::vector<std::vector<int>> movements;
    // std::vector<std::vector<int>> movements_inter = generate_all_vertex_movements_inter(cycle1.size());
    // movements.insert(movements.end(), movements_inter.begin(), movements_inter.end());
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

    // check new moves? - store in movements only those that are not in LM

    // czy ruch aplikowalny: - i,j: poprzednia good iteracja; i',j': obecna iteracja
    // reverse inside: if i' > i and j' < j
    // not applicable: if (i' < i and j' < j) or (i' > i and j' > j) or (i'=i or j'=j) (i out, j in) or (i in, j out)
    // good: if i' < i and j' > j (i out, j out)

    // int last_i = -1;
    // int last_j = 1000000;

    int start_len = calc_cycles_len();
    int n = cycle1.size();
    bool found_better;

    std::cout << movements.size() << std::endl;
    std::multiset<TupleType, TupleComparator> LM = init_LM(movements);
    std::cout << movements.size() << std::endl;
    std::cout << std::endl;
    int c = 0;
    do{
        found_better = false;
        update_LM(movements, LM);
        for (auto it = LM.begin(); it != LM.end();) {
            int i = std::get<0>(*it);
            int j = std::get<1>(*it);
            // int edge_or_vertex = std::get<2>(*it);
            int cycle_num = std::get<3>(*it);
            // int applicable = std::get<4>(*it);
            std::vector<int>& cycle = (cycle_num == 0) ? cycle1 : cycle2;
            std::vector<std::vector<int>>& adj_matrix = (cycle_num == 0) ? adj_matrix1 : adj_matrix2;

            // edges to delete
            int edge11 = cycle[(i - 1 + n) % n];
            int edge12 = cycle[i];
            int edge21 = cycle[j];
            int edge22 = cycle[(j + 1) % n];

            if (adj_matrix[edge11][edge12] == 0 || adj_matrix[edge21][edge22] == 0) {
                it = LM.erase(it);
                movements.push_back({i, j, 0, cycle_num});
                continue;
            }
            else if ((adj_matrix[edge11][edge12] == -1 && adj_matrix[edge21][edge22] == 1) ||
                (adj_matrix[edge11][edge12] == 1 && adj_matrix[edge21][edge22] == -1)){
                ++it;
                continue;
            }
            else{
                apply_movement({i, j, 0, 0, cycle_num});
                update_adj_matrix(adj_matrix, cycle, i, j);
                it = LM.erase(it);
                movements.push_back({i, j, 0, cycle_num});
                found_better = true;
                // last_i = i;
                // last_j = j;
                c++;
            }
        }
    }while(found_better);

    int end_len = calc_cycles_len();
    std::cout << start_len << " " << end_len << std::endl;
    std::cout << c <<std::endl;
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
