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
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <filename> <algotype> [nearest, expansion, regret, local]" 
        << " <input_data> [random, regret] <movements_type> [inner, inter] <steepest> [0, 1]" << std::endl;
        return 1;
    }
    Matrix m;
    m.load_from_path(argv[1]);
    m.generate_dist_matrix();

  

    //Parse parameters from command line
    LocalSearchParams params;
    params.input_data = std::string(argv[3]);
    params.movements_type = std::string(argv[4]);
    params.steepest = std::string(argv[5]);
    params.filename = argv[1];


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

    //std::cout << "I'm here" << std::endl;

    TSP tsp(m, alg_type, params.input_data, params.movements_type, params.steepest, params.filename);
    std::cout << "I'm here" << std::endl;

    generate_cycles(tsp);
    
    return 0;
}
