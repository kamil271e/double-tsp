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
    std::cout << m.x_coord.size() << std::endl;
    return 0;
}
