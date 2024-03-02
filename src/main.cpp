#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../lib/tsp.h"
#include "../lib/matrix.h"


int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    Matrix m;
    m.load_from_path(argv[1]);
    m.generate_dist_matrix();

    // Create TSP instance with the Greedy algorithm
    TSP tsp(m, AlgType::nearest_neighbors);

    // Solve the TSP problem
    auto [cycle1, cycle2] = tsp.solve();

    // Display the result for each cycle
    // std::cout << "Greedy TSP Cycle 1: ";
    for (int vertex : cycle1) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;

    // std::cout << "Greedy TSP Cycle 2: ";
    for (int vertex : cycle2) {
        std::cout << vertex + 1 << " ";
    }
    std::cout << std::endl;

    return 0;
}
