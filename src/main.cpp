#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../lib/tsp.h"
#include "../lib/matrix.h"


int main(int argc, char* argv[]){
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <algotype> [nearest, expansion, regret]" << std::endl;
        return 1;
    }
    Matrix m;
    m.load_from_path(argv[1]);
    m.generate_dist_matrix();

    AlgType alg_type;
    if (std::string(argv[2]) == "nearest") {
        alg_type = AlgType::nearest_neighbors;
    } else if (std::string(argv[2]) == "expansion") {
        alg_type = AlgType::greedy_cycle;
    } else if (std::string(argv[2]) == "regret") {
        alg_type = AlgType::regret_heuristic;
    } else {
        std::cerr << "Invalid algorithm type. Please choose from [nearest, expansion, regret]" << std::endl;
        return 1;
    }

    TSP tsp(m, alg_type);

    // Solve the TSP problem
    auto [cycle1, cycle2] = tsp.solve();

    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;

    for (int vertex : cycle2) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;

    return 0;
}
