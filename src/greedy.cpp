#include "../lib/tsp.h"

auto TSP::find_greedy_cycles() -> std::tuple<std::vector<int>, std::vector<int>> {
    
    auto startingVertices = choose_starting_vertices();
    int current_last_vertex1 = startingVertices.first;
    int current_last_vertex2 = startingVertices.second;
    int current_first_vertex1 = startingVertices.first;
    int current_first_vertex2 = startingVertices.second;

    append_vertex(current_last_vertex1, cycle1);
    append_vertex(current_last_vertex2, cycle2); 

    while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
        // Find nearest neighbors for the last and first vertices in each cycle
        auto nearest_neighbor_info1 = find_nearest_neighbor(current_last_vertex1, current_first_vertex1, visited);
        auto nearest_neighbor_info2 = find_nearest_neighbor(current_last_vertex2, current_first_vertex2, visited);

        int nearest_neighbor1 = nearest_neighbor_info1.first;
        int nearest_neighbor2 = nearest_neighbor_info2.first;
        int vertex_type1 = nearest_neighbor_info1.second;
        int vertex_type2 = nearest_neighbor_info2.second;

        std::vector<int>& cycle = (cycle1.size() <= cycle2.size()) ? cycle1 : cycle2;

        // Choose the nearest neighbor to the cycle that is shorter
        if (cycle1.size() <= cycle2.size()) {
            if (vertex_type1 == 0) {
                append_vertex(nearest_neighbor1, cycle);
                current_last_vertex1 = nearest_neighbor1;
            } else {
                insert_vertex(nearest_neighbor1, 0, cycle);
                current_first_vertex1 = nearest_neighbor1;
            }
        } else {
            if (vertex_type2 == 0) {
                append_vertex(nearest_neighbor2, cycle);
                current_last_vertex2 = nearest_neighbor2;
            } else {
                insert_vertex(nearest_neighbor2, 0, cycle);
                current_first_vertex2 = nearest_neighbor2;
            }
        }
    }

    return {cycle1, cycle2};
}


/* Finds the nearest neighbor of the current vertex that has not been visited yet.
 * 
 * @param current_vertex The index of the current vertex.
 * @param visited A vector indicating which vertices have been visited.
 * @return The index of the nearest neighbor.
 */
std::pair<int, int> TSP::find_nearest_neighbor(int current_last_vertex, int current_first_vertex, const std::vector<bool>& visited) {
    double min_distance_last = std::numeric_limits<double>::max();
    double min_distance_first = std::numeric_limits<double>::max();
    int nearest_neighbor_last = -1;
    int nearest_neighbor_first = -1;

    for (int i = 0; i < dist_matrix.x_coord.size(); ++i) {
        if (!visited[i] && i != current_last_vertex) {
            double distance = dist_matrix.dist_matrix[current_last_vertex][i];
            
            if (distance < min_distance_last) {
                min_distance_last = distance;
                nearest_neighbor_last = i;
            }
        }

        if (!visited[i] && i != current_first_vertex) {
            double distance = dist_matrix.dist_matrix[current_first_vertex][i];
            if (distance < min_distance_first) {
                min_distance_first = distance;
                nearest_neighbor_first = i;
            }
        }
    }

    // Zwraca parę, gdzie pierwszy element to indeks najbliższego sąsiada,
    // a drugi element to informacja o tym, dla którego wierzchołka został znaleziony najbliższy sąsiad
    if (min_distance_last < min_distance_first) {
        return {nearest_neighbor_last, 0}; // 0 oznacza, że sąsiad znaleziono dla ostatniego wierzchołka
    } else {
        return {nearest_neighbor_first, 1}; // 1 oznacza, że sąsiad znaleziono dla pierwszego wierzchołka
    }
}




/**
 * Finds the greedy cycles for the Traveling Salesman Problem (TSP).
 * 
 * This function uses a greedy algorithm to find two cycles that cover all vertices in the TSP graph.
 * The cycles are constructed by iterative process of cycle expansion.
 * 
 * @return A tuple containing two vectors representing two independent loops.
 */
auto TSP::find_greedy_cycles_expansion() -> std::tuple<std::vector<int>, std::vector<int>> {
    auto startingVertices = choose_starting_vertices();
    append_vertex(startingVertices.first, cycle1);
    append_vertex(startingVertices.second, cycle2);

    while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
        std::vector<int>& cycle = (cycle1.size() <= cycle2.size()) ? cycle1 : cycle2;

        double min_length = std::numeric_limits<double>::max();
        int best_candidate, best_idx = -1;

        for (size_t i = 0; i < cycle.size(); ++i) {
            int current = cycle[i];
            int next = cycle[(i + 1) % cycle.size()];
            auto [nearest, length] = find_nearest_expansion(current, next, visited);
            if (length < min_length){
                min_length = length;
                best_candidate = nearest;
                best_idx = i;
            }
        }

        insert_vertex(best_candidate, (best_idx + 1) % cycle.size(), cycle);
    }

    return {cycle1, cycle2};
}



/**
 * Finds the best candidate vertex between two for cycle expansion - leveraging minimum cycle length.
 * 
 * @param first First vertex.
 * @param last Last vertex.
 * @param visited A vector indicating which vertices have been visited.
 * @return Pair of: the index of the candidate vertex and the sum of distances between first and last.
 */
auto TSP::find_nearest_expansion(int first, int last, const std::vector<bool>& visited) -> std::pair<int, double>{
    if (first == last){
        return {find_nearest_neighbor(first, first, visited).first, 0.0};
    }

    double min_distance = std::numeric_limits<double>::max();
    int candidate = -1;

    for (int i = 0; i < dist_matrix.x_coord.size(); ++i) {
        if (!visited[i] && i != first && i != last) {
            double distance = dist_matrix.dist_matrix[first][i] + dist_matrix.dist_matrix[last][i] - dist_matrix.dist_matrix[first][last];
            if (distance < min_distance) {
                min_distance = distance;
                candidate = i;
            }
        }
    }

    return {candidate, min_distance};
}

auto TSP::find_greedy_regret_cycles(float coef) -> std::tuple<std::vector<int>, std::vector<int>> {
    auto startingVertices = choose_starting_vertices();
    append_vertex(startingVertices.first, cycle1);
    append_vertex(startingVertices.second, cycle2);

    while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
        std::vector<int>& cycle = (cycle1.size() <= cycle2.size()) ? cycle1 : cycle2;
        std::map<int, std::pair<int, double>> regrets; // indx, regret

        for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
            if (!visited[i]) {
                std::vector<std::pair<int, double>> cost_i; // idx, distance
                for (size_t j = 0; j < cycle.size(); ++j) {
                    double distance = dist_matrix.dist_matrix[cycle[j]][i] + dist_matrix.dist_matrix[i][cycle[(j + 1) % cycle.size()]] - dist_matrix.dist_matrix[cycle[j]][cycle[(j + 1) % cycle.size()]];
                    cost_i.push_back({j, distance});
                }
                // Find the indices of the two lowest values
                int min_idx = 0;
                int second_min_idx = 1;
                if (cost_i[1].second < cost_i[min_idx].second) {
                    min_idx = 1;
                    second_min_idx = min_idx;
                }

                for (size_t k = 2; k < cost_i.size(); ++k) {
                    if (cost_i[k].second < cost_i[min_idx].second) {
                        second_min_idx = min_idx;
                        min_idx = k;
                    } else if (cost_i[k].second < cost_i[second_min_idx].second) {
                        second_min_idx = k;
                    }
                }


                double regret = cost_i[second_min_idx].second - cost_i[min_idx].second;
                double weight = regret - coef * cost_i[min_idx].second;

                // Store the best candidate and its regret in the regrets map
                regrets[i] = {cost_i[min_idx].first, weight};
            }
        }

        // Find the vertex with the maximum regret among unvisited vertices
        int candidate = -1;
        double max_regret = -std::numeric_limits<double>::infinity();

        for (const auto& r : regrets) {
            if (r.second.second > max_regret) {
                max_regret = r.second.second;
                candidate = r.first;
            }
        }

        // Insert the vertex with the maximum regret after the vertex with the best insertion position
        insert_vertex(candidate, (regrets[candidate].first + 1) % cycle.size(), cycle);

    }

    return {cycle1, cycle2};
}

/**
 * Finds the k-best candidates vertices between two for cycle expansion - leveraging minimum cycle length.
 * 
 * @param first First vertex.
 * @param last Last vertex.
 * @param visited A vector indicating which vertices have been visited.
 * @return Pair of: the index of the candidate vertex and the 2-regret value.
 */
auto TSP::find_best_2regret(int first, int last, const std::vector<bool>& visited) -> std::pair<int, double>{
    double k_best_distances[2] = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
    int candidate = -1;

    for (int i = 0; i < dist_matrix.x_coord.size(); ++i) {
        if (!visited[i] && i != first && i != last) {
            double distance = dist_matrix.dist_matrix[first][i] + dist_matrix.dist_matrix[last][i] - dist_matrix.dist_matrix[first][last];
            if (distance < k_best_distances[1]) {
                if (distance < k_best_distances[0]) {
                    candidate = i;
                    k_best_distances[1] = k_best_distances[0];
                    k_best_distances[0] = distance;
                } else {
                    k_best_distances[1] = distance;
                }
            }
        }
    }
    double regret = k_best_distances[1] - k_best_distances[0];
    return {candidate, regret};
}