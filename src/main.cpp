#include <iostream>
#include <vector>
#include "../lib/tsp.h"


void generate_cycles(TSP tsp){
    auto [cycle1, cycle2] = tsp.solve();

    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;

    for (int vertex : cycle2) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]){
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <algotype> {nearest, expansion, regret, local} [start_idx]" << std::endl;
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
        alg_type = AlgType::regret;
    } else if (std::string(argv[2]) == "local") {
        alg_type =  AlgType::local;
    } else {
        std::cerr << "Invalid algorithm type. Please choose from [nearest, expansion, regret, local]" << std::endl;
        return 1;
    }

    TSP tsp(m, alg_type);
    generate_cycles(tsp);

    return 0;
}
