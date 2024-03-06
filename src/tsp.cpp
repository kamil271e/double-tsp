#include "../lib/tsp.h"

TSP::TSP(const Matrix& dist_matrix, AlgType alg_type)
    : dist_matrix(dist_matrix), alg_type(alg_type) {
}

auto TSP::solve() -> std::tuple<std::vector<int>, std::vector<int>>{
    switch (alg_type) {
        case AlgType::nearest_neighbors:
            // return find_greedy_cycle();
            return find_greedy_cycles();
            break;
        case AlgType::greedy_cycle:
            return find_greedy_cycles_expansion();
            break;
        // Add more algorithms as needed in the future
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

    for (int i = 0; i < dist_matrix.x_coord.size(); ++i) {
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
    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;

    std::cout << "TSP Cycle 2: ";
    for (int vertex : cycle2) {
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
    int start1 = find_random_start();
    int start2 = find_farthest(start1);
    return {start1, start2};
}

/**
 * Finds the greedy cycles for the Traveling Salesman Problem (TSP).
 * 
 * This function uses a greedy algorithm to find two cycles that cover all vertices in the TSP graph.
 * The cycles are constructed by iteratively choosing the nearest neighbor for each vertex.
 * 
 * @return A tuple containing two vectors representing two independent loops.
 */
auto TSP::find_greedy_cycles() -> std::tuple<std::vector<int>, std::vector<int>> {
    
    auto startingVertices = choose_starting_vertices();
    int current_vertex1 = startingVertices.first;
    int current_vertex2 = startingVertices.second;
    append_vertex(current_vertex1, cycle1);
    append_vertex(current_vertex2, cycle2);

    while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
        int nearest_neighbor1 = find_nearest_neighbor(current_vertex1, visited);
        int nearest_neighbor2 = find_nearest_neighbor(current_vertex2, visited);

        // Choose the closer neighbor
        if ((cycle1.size() < dist_matrix.x_coord.size() / 2 && dist_matrix.x_coord.size() % 2 == 0) ||
            (cycle2.size() < dist_matrix.x_coord.size() / 2)) {
            // Choose the nearest neighbor to the cycle that is shorter
            if (cycle1.size() <= cycle2.size()) {
                append_vertex(nearest_neighbor1, cycle1);
                current_vertex1 = nearest_neighbor1;
            } else {
                append_vertex(nearest_neighbor2, cycle2);
                current_vertex2 = nearest_neighbor2;
            }
        } else {
            // Both cycles have at least half of the vertices, choose any
            if (dist_matrix.dist_matrix[current_vertex1][nearest_neighbor1] <
                dist_matrix.dist_matrix[current_vertex2][nearest_neighbor2]) {
                append_vertex(nearest_neighbor1, cycle1);
                current_vertex1 = nearest_neighbor1;
            } else {
                append_vertex(nearest_neighbor2, cycle2);
                current_vertex2 = nearest_neighbor2;
            }
        }
    }

    return {cycle1, cycle2};
}


/**
 * Finds the nearest neighbor of the current vertex that has not been visited yet.
 * 
 * @param current_vertex The index of the current vertex.
 * @param visited A vector indicating which vertices have been visited.
 * @return The index of the nearest neighbor.
 */
int TSP::find_nearest_neighbor(int current_vertex, const std::vector<bool>& visited) {
    double min_distance = std::numeric_limits<double>::max();
    int nearest_neighbor = -1;

    for (int i = 0; i < dist_matrix.x_coord.size(); ++i) {
        if (!visited[i] && i != current_vertex) {
            double distance = dist_matrix.dist_matrix[current_vertex][i];
            if (distance < min_distance) {
                min_distance = distance;
                nearest_neighbor = i;
            }
        }
    }

    return nearest_neighbor;
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
        std::vector<int> candidates1;
        std::vector<int> candidates2;
        std::vector<double> lengths1;
        std::vector<double> lengths2;

        // Iterate over each pair of neighbor nodes in cycles and find the nearest expansion candidate
        for (size_t i = 0; i < cycle1.size(); ++i) {
            int current1 = cycle1[i];
            int next1 = cycle1[(i + 1) % cycle1.size()];
            auto [nearest1, length1] = find_nearest_expansion(current1, next1, visited);
            candidates1.push_back(nearest1);
            lengths1.push_back(length1);
        }
        for (size_t i = 0; i < cycle2.size(); ++i) {
            int current2 = cycle2[i];
            int next2 = cycle2[(i + 1) % cycle2.size()];
            auto [nearest2, length2] = find_nearest_expansion(current2, next2, visited);
            candidates2.push_back(nearest2);
            lengths2.push_back(length2);
        }

        // Find the best expansion candidate for each cycle
        double min_length1 = std::numeric_limits<double>::max();
        double min_length2 = std::numeric_limits<double>::max();
        int best_candidate1, best_candidate2 = -1;
        int best_idx1, best_idx2 = -1;

        for (size_t i = 0; i < lengths1.size(); ++i) {
            if (lengths1[i] < min_length1) {
                min_length1 = lengths1[i];
                best_candidate1 = candidates1[i];
                best_idx1 = i;
            }
        }

        for (size_t i = 0; i < lengths2.size(); ++i) {
            if (lengths2[i] < min_length2) {
                min_length2 = lengths2[i];
                best_candidate2 = candidates2[i];
                best_idx2 = i;
            }
        }

        if (cycle1.size() <= cycle2.size()) {
            insert_vertex(best_candidate1, (best_idx1 + 1) % cycle1.size(), cycle1);
        } else {
            insert_vertex(best_candidate2, (best_idx2 + 1) % cycle2.size(), cycle2);
        }
        // log_build_process();
    }

    return {cycle1, cycle2};
}


/**
 * Finds the best candidate vertex between two for cycle expansion - leveraging minimum cycle length.
 * 
 * @param first First vertex.
 * @param last Last vertex.
 * @param visited A vector indicating which vertices have been visited.
 * @return Pait of: the index of the candidate vertex and the sum of distances between first and last.
 */
auto TSP::find_nearest_expansion(int first, int last, const std::vector<bool>& visited) -> std::pair<int, double>{
    if (first == last){
        return {find_nearest_neighbor(first, visited), 0};
    }

    double min_distance = std::numeric_limits<double>::max();
    int candidate = -1;

    for (int i = 0; i < dist_matrix.x_coord.size(); ++i) {
        if (!visited[i] && i != first && i != last) {
            double distance = dist_matrix.dist_matrix[first][i] + dist_matrix.dist_matrix[last][i];
            if (distance < min_distance) {
                min_distance = distance;
                candidate = i;
            }
        }
    }

    return {candidate, min_distance};
}