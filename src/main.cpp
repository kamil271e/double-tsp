#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../lib/tsp.h"
#include "../lib/matrix.h"
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
        std::cerr << "Usage: " << argv[0] << " <filename> <algotype> [nearest, expansion, regret, local]" << std::endl;
        return 1;
    }
    Matrix m;
    m.load_from_path(argv[1]);
    m.generate_dist_matrix();

    //Print argv[2])
    std::cout << "Algorithm type: " << argv[2] << std::endl;

    AlgType alg_type;
    if (std::string(argv[2]) == "nearest") {
        alg_type = AlgType::nearest_neighbors;
    } else if (std::string(argv[2]) == "expansion") {
        alg_type = AlgType::greedy_cycle;
    } else if (std::string(argv[2]) == "regret") {
        alg_type = AlgType::regret;
    } else if (std::string(argv[2]) == "local") {
        //Run local search
        alg_type =  AlgType::local;
    
    } else {
        std::cerr << "Invalid algorithm type. Please choose from [nearest, expansion, regret, local]" << std::endl;
        return 1;
    }

    TSP tsp(m, alg_type);
    auto [cycle1, cycle2] = tsp.solve();

    std::cout << "Cycle 1: ";
    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;

    std::cout << "Cycle 2: ";
    for (int vertex : cycle2) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
