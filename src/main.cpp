#include "../src/matrix.cpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    Matrix m;
    m.load_from_path(argv[1]);
    m.generate_dist_matrix();
    // std::cout << m.x_coord.size() << std::endl;
    for (int i = 0; i < 5; i++){
        std::cout << m.dist_matrix[99][99-i] << " ";
    }
    return 0;
}
