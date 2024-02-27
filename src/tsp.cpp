#include "../lib/tsp.h"

TSP::TSP(const Matrix& dist_matrix, AlgType alg_type)
    : dist_matrix(dist_matrix), alg_type(alg_type) {
}

std::vector<int> TSP::solve() {
    switch (alg_type) {
        case AlgType::nearest_neighbors:
            return find_greedy_cycle();
            break;
        // Add more algorithms as needed in the future
        default:
            // Handle unsupported algorithm type
            break;
    }
    return cycle;
}

int TSP::find_random_start() {
    return rand() % dist_matrix.x_coord.size();
}

int TSP::find_farthest(int node) { return 0;}

int TSP::find_nearest(int node) {return 0;}

int TSP::calc_distance(int node1, int node2) {return 0;}

std::vector<int> TSP::find_greedy_cycle() {
    int current_vertex = find_random_start();
    std::vector<int> cycle;

    cycle.push_back(current_vertex);
    visited[current_vertex] = true;

    while (cycle.size() < dist_matrix.x_coord.size()) {
        int nearest_neighbor = find_nearest_neighbor(current_vertex, visited);
        cycle.push_back(nearest_neighbor);
        visited[nearest_neighbor] = true;
        current_vertex = nearest_neighbor;
    }

    return cycle;
}

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

