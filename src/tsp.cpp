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

auto TSP::find_greedy_regret_cycles() -> std::tuple<std::vector<int>, std::vector<int>> {
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
                double weight = regret - 0.42 * cost_i[min_idx].second;

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



// iterowanie sie po elementach nie odwiedzonych
// sprawdzanie kosztow dla kazdego z dwoch par w cyklu
// wybranie tego wierzcholka dla ktorego regret jest najwiekszy
// wstawienie go w odpowiednie miejsce

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

//-----------------LOCAL SEARCH-----------------//
// Implement read cycle function
auto TSP::read_cycle(const std::string& file) -> std::vector<std::vector<int>>
{
    std::ifstream inputFile(file);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return {};
    }

    std::vector<std::vector<int>> cycles;

    std::string line;
    while (std::getline(inputFile, line)) {
        std::vector<int> cycle;
        std::istringstream ss(line);
        int num;
        while (ss >> std::setbase(10) >> num) {
            cycle.push_back(num-1);
        }
        cycles.push_back(cycle);
    }

    inputFile.close();

    return cycles;
}

// Function to replacement of two vertices included in the path
auto TSP::generate_neighbors(const std::vector<int>& x, int n) -> std::vector<std::vector<int>>{

    std::vector<std::vector<int>> neighbors;
    std::set<std::vector<int>> unique_neighbors;
    std::random_device rd;
    std::mt19937 gen(rd());

    while (neighbors.size() < n) {
        std::uniform_int_distribution<> dis_i(0, x.size() - 2);
        std::uniform_int_distribution<> dis_j(1, x.size() - 1);

        int i = dis_i(gen);
        int j = dis_j(gen);

        if (i > j)
            std::swap(i, j);

        std::vector<int> neighbor = x;
        std::reverse(neighbor.begin() + i, neighbor.begin() + j);

        if (unique_neighbors.find(neighbor) == unique_neighbors.end()) {
            neighbors.push_back(neighbor);
            unique_neighbors.insert(neighbor);
        }
    }

    // Return the vector of neighbors
    return neighbors;
}


// Function to generate all possible neighbors by swapping each edge replacement
auto TSP::generate_all_edge_movements(const std::vector<int>& x) -> std::vector<std::vector<int>> {
    // edge movement : type = 0
    // vertx movement: type = 1
    std::vector<std::vector<int>> neighbors; // i, j, type
    int n = x.size();

    // Generate all possible neighbors by swapping each pair of vertices
    for (int i = 0; i < n; ++i) {
        for (int j = i + 2; j < n; ++j) {
            if (i==0 && j==n-1) continue;
            neighbors.push_back({i, j, 0});
        }
    }

    return neighbors;
}

auto TSP::generate_all_vertex_movements(const std::vector<int>& x) -> std::vector<std::vector<int>> {
    std::set<std::vector<int>> unique_neighbors;
    std::vector<std::vector<int>> neighbors; // i,j,id

    int n = x.size();

    // Generate all possible neighbors by swapping each pair of vertices
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            std::vector<int> neighbor = x;
            std::swap(neighbor[i], neighbor[j]);
            if (unique_neighbors.find(neighbor) == unique_neighbors.end()) {
                    neighbors.push_back(neighbor);
                    unique_neighbors.insert(neighbor);
                }

        }
    }

    return neighbors;
}

// Function to calculate the delta of the path / objective value
float TSP::get_objective_value(const std::vector<int>& cycle, std::vector<int> neighbor) {
    int i0 = neighbor[0];
    int j0 = neighbor[1];
    int n = cycle.size();
//    int i1 = i0 - 1; // c++ not support negative indices! : -1
//    int j1 = j0 + 1;
//    if (i1 < 0) i1 = n - 1;
//    if (j1 > n - 1) j1 = 0;
    int i1 = (i0 - 1 + n) % n;
    int j1 = (j0 + 1) % n;

    if (neighbor[2] == 0){ // edge
        // i = 2
        // j = 5
        // 1 2 [3] 4 5 [6] 7
        // 1 2 [6] 5 4 [3] 7
        // new 3,7 i 2,6:     i, (j+1)%size --- (i-1)%size, j
        // delete 2,3 i 6,7: (i-1)%size, i  --- (j+1)%size, j
        float deleted = dist_matrix.dist_matrix[cycle[i0]][cycle[i1]] + dist_matrix.dist_matrix[cycle[j0]][cycle[j1]];
        float added = dist_matrix.dist_matrix[cycle[i0]][cycle[j1]] + dist_matrix.dist_matrix[cycle[i1]][cycle[j0]];
        float delta = deleted - added;
        std::cout << "delta: " << delta << std::endl;
        return delta;
    } else{ // vertex
        // TODO
    }
    return 0;
}

auto TSP::find_random_neighbor(std::vector<std::vector<int>> neighbors) -> std::vector<int> {
    // Choose a random neighbor
    return neighbors[std::rand() % neighbors.size()];
}

auto TSP::hill_climbing(const std::vector<int>& init_cycle, bool steepest = false) -> std::vector<int>
{
    // Generate neighbors
    std::vector<std::vector<int>> egde_movements;
    egde_movements = generate_all_edge_movements(init_cycle);

    std::vector<std::vector<int>> neighbors;
    neighbors.insert(neighbors.end(), egde_movements.begin(), egde_movements.end());

    //std::vector<std::vector<int>> vertex_movements;
    //vertex_movements = generate_all_vertex_movements(init_cycle);
    //neighbors.insert(neighbors.end(), vertex_movements.begin(), vertex_movements.end());

    std::vector<int> cycle = init_cycle;
    std::cout << "CYCLE LENGTH BEFORE: " << calc_cycle_len(cycle) << std::endl;

    for (int iter = 0; iter < neighbors.size(); ++iter) { // for now typical iteration, later we need to chose them randomly
        if (get_objective_value(cycle, neighbors[iter]) > 0) { // better than current
            int i = neighbors[iter][0];
            int j = neighbors[iter][1];
            int type = neighbors[iter][2];
            if (steepest) {
                // modify cycle and continue
            } else {
                if (type == 0){ // edge
//                    std::cout << "REVERSE" << std::endl;
                    std::reverse(cycle.begin() + i, cycle.begin() + j + 1);
                    break;
                }else{// vertex

                }
                break;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "CYCLE LENGTH AFTER: " << calc_cycle_len(cycle) << std::endl;
    return cycle;

};

// Function to perform local search
auto TSP::local_search() -> std::tuple<std::vector<int>, std::vector<int>>
{
    // TODO: we should be able to choose starting cycles
    // auto [cycle1, cycle2] = find_greedy_cycles();
    auto [cycle1, cycle2] = generate_random_cycles(100);

    std::vector<int> hill_cycle1;
    std::vector<int> hill_cycle2;

    std::cout << "INITIAL CYCLES: " << std::endl;
    std::cout << "Cycle 1: ";
    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
//    std::cout << "Cycle 2: ";
//    for (int vertex : cycle2) {
//        std::cout << vertex + 1 << " ";
//    }
//    std::cout << std::endl;


    std::cout << "START LOCAL SEARCH" << std::endl;
    hill_cycle1 = hill_climbing(cycle1, false);
//    hill_cycle2 = hill_climbing(cycle2, false);

    return std::make_tuple(hill_cycle1, hill_cycle2);

};

auto TSP::generate_random_cycles(int n) -> std::tuple<std::vector<int>, std::vector<int>>
{
    std::vector<int> values(n);
    for (int i = 0; i < n; ++i) values[i] = i;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(values.begin(), values.end(), g);

    std::vector<int> cycle1(values.begin(), values.begin() + (int)(n/2));
    std::vector<int> cycle2(values.begin() + n - (int)(n/2), values.end());

    return {cycle1, cycle2};
};