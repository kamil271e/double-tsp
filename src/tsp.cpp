#include "../lib/tsp.h"


TSP::TSP(const Matrix& dist_matrix, AlgType alg_type)
        : dist_matrix(dist_matrix), alg_type(alg_type){
}


TSP::TSP(const Matrix& dist_matrix, AlgType alg_type, std::string input_data, std::string movements_type, int steepest, std::string filename)
        : dist_matrix(dist_matrix), alg_type(alg_type), params({input_data, movements_type, filename, steepest}){
}


auto TSP::solve() -> std::tuple<std::vector<int>, std::vector<int>>{
    switch (alg_type) {
        case AlgType::nearest_neighbors:
            return find_greedy_cycles_nearest();
        case AlgType::greedy_cycle:
            return find_greedy_cycles_expansion();
        case AlgType::regret:
            return find_greedy_cycles_regret();
        case AlgType::local:
            return local_search();
        default:
            // Handle unsupported algorithm type
            break;
    }
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


std::pair<int, int> TSP::choose_starting_vertices() {
    int start1 = (this->start_idx == -1) ? find_random_start() : this->start_idx;
    int start2 = find_farthest(start1);
    return {start1, start2};
}


double TSP::calc_cycles_len() {
    int len1 = 0;
    int len2 = 0;
    for (int i = 0; i < cycle1.size(); ++i) {
        len1 += calc_distance(cycle1[i], cycle1[(i + 1) % cycle1.size()]);
        len2 += calc_distance(cycle2[i], cycle2[(i + 1) % cycle2.size()]);
    }
    return len1 + len2;
}


auto TSP::generate_random_cycles(int n) -> std::tuple<std::vector<int>, std::vector<int>>
{
    std::vector<int> values(n);
    for (int i = 0; i < n; ++i) values[i] = i;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(values.begin(), values.end(), g);

    std::vector<int> c1(values.begin(), values.begin() + (int)(n/2));
    std::vector<int> c2(values.begin() + n - (int)(n/2), values.end());

    return {c1, c2};
}

void createFileIfNotExists(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::ofstream outfile(filePath);
        outfile.close();
    } else {
        file.close();
    }
}

// TODO - more generic, we will use it for all algorithms
void TSP::save_time_and_length(long duration, struct LocalSearchParams params) {
        std::string steepest = (params.steepest == 1) ? "steepest" : "greedy";
        std::string cycles_time_file = "../cycles/T_local_" + params.input_data + "_" + params.movements_type + "_" + steepest + "_" + params.filename.substr(0, params.filename.size() - 4) + ".txt";
        std::string cycles_length_file = "../cycles/L_local_" + params.input_data + "_" + params.movements_type + "_" + steepest + "_" + params.filename.substr(0, params.filename.size() - 4) + ".txt";

        createFileIfNotExists(cycles_time_file);
        createFileIfNotExists(cycles_length_file);

        std::ofstream outfile(cycles_time_file, std::ios_base::app);
        if (outfile.is_open()) {
            outfile << duration << std::endl;
            outfile.close();
        } else {
            std::cerr << "Failed to open " << cycles_time_file << " for writing." << std::endl;
        }

        std::ofstream out_len_file(cycles_length_file, std::ios_base::app);
        if (out_len_file.is_open()) {
            out_len_file << calc_cycles_len() << std::endl;
            out_len_file.close();
        } else {
            std::cerr << "Failed to open " << cycles_length_file << " for writing." << std::endl;
        }
}

