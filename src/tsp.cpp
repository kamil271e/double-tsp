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
        case AlgType::multiple_local_search:
            return multiple_local_search();
        case AlgType::ils1:
            return iterative_local_search_one();
        case AlgType::ils2:
            return iterative_local_search_two();
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
void TSP::save_time(long duration, struct LocalSearchParams params, std::string algo) {
    std::string steepest = (params.steepest == 1) ? "steepest" : "greedy";
    std::string cycles_time_file = "../cycles/T_" + algo +"_" + params.input_data + "_" + params.movements_type + "_" + steepest + "_" + params.filename.substr(0, params.filename.size() - 4) + ".txt";
    createFileIfNotExists(cycles_time_file);
    std::ofstream outfile(cycles_time_file, std::ios_base::app);
    outfile << duration << std::endl;
    outfile.close();
}

auto calculateAverageFromFile(std::ifstream& file) {
    long long sum = 0.0;
    int count = 0;

    // Calculate sum and count
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double value;
        while (iss >> value) {
            sum += value;
            count++;
        }
    }

    file.close();

    if (count == 0) {
        std::cerr << "No values found in the file." << std::endl;
        return std::chrono::steady_clock::now(); // Or handle this case in some other way
    }

    // Calculate average time
    long long avg_time = sum / count;
    std::chrono::milliseconds duration(avg_time);
    auto avg_time_in_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    auto target_time = std::chrono::steady_clock::now() + avg_time_in_milliseconds;

    return target_time;
}

std::chrono::steady_clock::time_point TSP::calculateAverageMSLStime(){
    
    //Find the average time from the file
    std:: string algo = "msls";
    std::string steepest = (params.steepest == 1) ? "steepest" : "greedy";
    std::string cycles_time_file = "../cycles/T_" + algo +"_" + params.input_data + "_" + params.movements_type + "_" + steepest + "_" + params.filename.substr(0, params.filename.size() - 4) + ".txt";
    std::ifstream  file(cycles_time_file);

    //std::cout << "Calculating average time from file: " << cycles_time_file << std::endl;

    //Check if the file exists
    if (!file.is_open()){
        std::cerr << "Error: File not found." << std::endl;
        std::cerr << "Please use the following script to generate data: " << std::endl;
        std::cerr << "./repeat.sh " << params.filename <<" "<< algo << " " << params.input_data << " " << params.movements_type << " " << params.steepest << "\n" << std::endl;
        exit(EXIT_FAILURE);
    }

    auto target_time = calculateAverageFromFile(file);

    return target_time;
}


// Function to calculate objective value of the cycle
int TSP::calculate_objective(const std::vector<int>& c1, const std::vector<int>& c2)
{
    int objective_value = 0;
    for (size_t i = 0; i < c1.size(); ++i) {
        objective_value += dist_matrix.dist_matrix[c1[i]][c1[(i + 1) % c1.size()]];
        objective_value += dist_matrix.dist_matrix[c2[i]][c2[(i + 1) % c2.size()]];
    }
   
    return objective_value;

}

// Temporary solution to the problem of duplicate vertices in the cycle
// Function to delete duplicate vertices in the cycle
auto TSP::delete_duplicates(const std::vector<int>& cycle) -> std::vector<int>
{
    std::vector<int> new_cycle;
    for (int i = 0; i < cycle.size(); ++i) {
        if (std::find(new_cycle.begin(), new_cycle.end(), cycle[i]) == new_cycle.end()) {
            new_cycle.push_back(cycle[i]);
        }
    }
    return new_cycle;
}