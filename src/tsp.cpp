#include "../lib/tsp.h"


TSP::TSP(const Matrix& dist_matrix, AlgType alg_type)
    : dist_matrix(dist_matrix), alg_type(alg_type) {
}

auto TSP::solve() -> std::tuple<std::vector<int>, std::vector<int>>{
    switch (alg_type) {
        case AlgType::nearest_neighbors:
            return find_greedy_cycles();
            break;
        case AlgType::greedy_cycle:
            return find_greedy_cycles_expansion();
            break;
        case AlgType::regret:
            return find_greedy_regret_cycles();
            break;
        case AlgType::local:
            return local_search();
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

double TSP::calc_cycle_len(const std::vector<int>& cycle){
    int len = 0;
    for (int i =0 ; i< cycle.size(); ++i){
        len+= calc_distance(cycle[i],cycle[(i+1)%cycle.size()]);
    }
    return len;
}



