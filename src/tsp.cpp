#include "../lib/tsp.h"

TSP::TSP(const Matrix& dist_matrix, AlgType alg_type, int start_idx)
    : dist_matrix(dist_matrix), alg_type(alg_type), start_idx(start_idx) {
}

auto TSP::solve() -> std::tuple<std::vector<int>, std::vector<int>>{
    switch (alg_type) {
        case AlgType::nearest_neighbors:
            return find_greedy_cycles_nearest();
            break;
        case AlgType::greedy_cycle:
            return find_greedy_cycles_expansion();
            break;
        case AlgType::regret:
            return find_greedy_cycles_regret();
            break;
        default:
            // Handle unsupported algorithm type
            break; 
    }

    // Return empty cycles if no algorithm was found
    return {cycle1, cycle2};
}

int TSP::find_random_start() {
    // return rand() % dist_matrix.x_coord.size();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, dist_matrix.x_coord.size() - 1);
    return dist(gen);
}   

int TSP::find_farthest(int node) {
    int farthest_node = -1;
    double max_distance = -1.0;

    for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
        if (i != node) {
            double distance = calc_distance(node, i);

            if (distance > max_distance) {
                max_distance = distance;
                farthest_node = i;
            }
        }
    }

    return farthest_node;
}

int TSP::calc_distance(int v1, int v2) {
    return std::sqrt(std::pow(dist_matrix.x_coord[v2] - dist_matrix.x_coord[v1], 2) +
                     std::pow(dist_matrix.y_coord[v2] - dist_matrix.y_coord[v1], 2));
}

void TSP::append_vertex(int v, std::vector<int>& cycle){
    cycle.push_back(v);
    visited[v] = true;
}

void TSP::insert_vertex(int v, int pos, std::vector<int>& cycle){
    cycle.insert(cycle.begin() + pos, v);
    visited[v] = true;
}

void TSP::log_build_process(){
    std::cout << "TSP Cycle 1: ";
    for (size_t vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;

    std::cout << "TSP Cycle 2: ";
    for (size_t vertex : cycle2) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
    std::cout << "-----------------" << std::endl;
}

/**
 * This function finds two starting vertices for the TSP algorithm. The first vertex is chosen randomly,
 * while the second vertex is chosen as the farthest vertex from the first vertex.
 * 
 * @return A pair of integers representing the chosen starting vertices.
 */
std::pair<int, int> TSP::choose_starting_vertices() {
    int start1 = (this->start_idx == -1) ? find_random_start() : this->start_idx;
    int start2 = find_farthest(start1);
    return {start1, start2};
}

auto TSP::find_greedy_cycles_nearest() -> std::tuple<std::vector<int>, std::vector<int>> {
    auto start = choose_starting_vertices();
    int current_last_vertex1 = start.first;
    int current_last_vertex2 = start.second;
    int current_first_vertex1 = start.first;
    int current_first_vertex2 = start.second;

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

    for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
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
    auto start = choose_starting_vertices();
    append_vertex(start.first, cycle1);
    append_vertex(start.second, cycle2);

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


double TSP::get_expansion_cost(int first, int last, int candidate){
    return dist_matrix.dist_matrix[first][candidate] + dist_matrix.dist_matrix[last][candidate] - dist_matrix.dist_matrix[first][last];
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

    for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
        if (!visited[i] && i != first && i != last) {
            double distance = get_expansion_cost(first, last, i);
            if (distance < min_distance) {
                min_distance = distance;
                candidate = i;
            }
        }
    }

    return {candidate, min_distance};
}

auto TSP::find_greedy_cycles_regret() -> std::tuple<std::vector<int>, std::vector<int>> {
    auto start = choose_starting_vertices();
    append_vertex(start.first, cycle1);
    append_vertex(start.second, cycle2);
    std::map<int, std::pair<int, double>> regrets;

    while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
        std::vector<int>& cycle = (cycle1.size() <= cycle2.size()) ? cycle1 : cycle2;
        regrets.clear();

        for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
            if (!visited[i]) {
                regrets[i] = get_2regret(i, cycle);
            }
        }

        int candidate = -1;
        double max_regret = -std::numeric_limits<double>::infinity();

        for (const auto& r : regrets) {
            if (r.second.second > max_regret) {
                max_regret = r.second.second;
                candidate = r.first;
            }
        }
        insert_vertex(candidate, (regrets[candidate].first + 1) % cycle.size(), cycle);
    }

    return {cycle1, cycle2};
}


/**
 * Finds the best candidate vertex based on weighted 2-regret value.
 * 
 * @param candidate Examined vertex index.
 * @param cycle Examined cycle.
 * @return Pair of: the index of the candidate vertex and the weighted regret of candidate.
 */
auto TSP::get_2regret(int candidate, std::vector<int> cycle) -> std::pair<int, double>{
    float coef = 0.42; // empirically chosen coefficient
    std::vector<std::pair<int, double>> cost; // idx, distance
    for (size_t i = 0; i < cycle.size(); ++i) {
        double distance = get_expansion_cost(cycle[i], cycle[(i + 1) % cycle.size()], candidate);
        cost.push_back({i, distance});
    }
    // Find the indices of the two lowest values
    int min_idxs[2];
    min_idxs[0] = cost[0].second < cost[1].second ? 0 : 1;
    min_idxs[1] = 1 - min_idxs[0];

    for (size_t i = 2; i < cost.size(); ++i) {
        if (cost[i].second < cost[min_idxs[0]].second) {
            min_idxs[1] = min_idxs[0];
            min_idxs[0] = i;
        } else if (cost[i].second < cost[min_idxs[1]].second) {
            min_idxs[1] = i;
        }
    }

    double regret = cost[min_idxs[1]].second - cost[min_idxs[0]].second;
    double weighted_regret = regret - coef * cost[min_idxs[0]].second;

    return {cost[min_idxs[0]].first, weighted_regret};
}
