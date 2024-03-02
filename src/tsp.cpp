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
        // Add more algorithms as needed in the future
        default:
            // Handle unsupported algorithm type
            break; 
    }

    // Return empty cycles if no algorithm was found
    return {cycle1, cycle2};
}

int TSP::find_random_start() {
    return rand() % dist_matrix.x_coord.size();
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

int TSP::calc_distance(int node1, int node2) {
    return std::sqrt(std::pow(dist_matrix.x_coord[node2] - dist_matrix.x_coord[node1], 2) +
                     std::pow(dist_matrix.y_coord[node2] - dist_matrix.y_coord[node1], 2));
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

    cycle1.push_back(current_vertex1);
    cycle2.push_back(current_vertex2);

    visited[current_vertex1] = true;
    visited[current_vertex2] = true;

    while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
        int nearest_neighbor1 = find_nearest_neighbor(current_vertex1, visited);
        int nearest_neighbor2 = find_nearest_neighbor(current_vertex2, visited);

        // Choose the closer neighbor
        if ((cycle1.size() < dist_matrix.x_coord.size() / 2 && dist_matrix.x_coord.size() % 2 == 0) ||
            (cycle2.size() < dist_matrix.x_coord.size() / 2)) {
            // Choose the nearest neighbor to the cycle that is shorter
            if (cycle1.size() <= cycle2.size()) {
                cycle1.push_back(nearest_neighbor1);
                visited[nearest_neighbor1] = true;
                current_vertex1 = nearest_neighbor1;
            } else {
                cycle2.push_back(nearest_neighbor2);
                visited[nearest_neighbor2] = true;
                current_vertex2 = nearest_neighbor2;
            }
        } else {
            // Both cycles have at least half of the vertices, choose any
            if (dist_matrix.dist_matrix[current_vertex1][nearest_neighbor1] <
                dist_matrix.dist_matrix[current_vertex2][nearest_neighbor2]) {
                cycle1.push_back(nearest_neighbor1);
                visited[nearest_neighbor1] = true;
                current_vertex1 = nearest_neighbor1;
            } else {
                cycle2.push_back(nearest_neighbor2);
                visited[nearest_neighbor2] = true;
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

