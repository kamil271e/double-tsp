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
    std::vector<int> greedyPath = tsp.solve();

    // Display the result
    std::cout << "Greedy TSP cycle: ";
    for (int vertex : greedyPath) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;

    return 0;
}
