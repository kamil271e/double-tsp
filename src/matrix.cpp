#include "../lib/matrix.h"
#include "../src/utils.cpp"

Matrix::Matrix() {}

void Matrix::load_from_path(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    std::string line;
    bool reading_coords = false;
    while (std::getline(file, line)) {
        if (line == "NODE_COORD_SECTION") {
            reading_coords = true;
            continue;
        }
        if (line == "EOF") {
            std::cout << "EOF" << std::endl;
            break;
        }
        if (!reading_coords) {
            continue;
        }
        std::vector<std::string> tokens = splitString(line, ' ');
        if (tokens.size() >= 3) {
            x_coord.push_back(std::stoi(tokens[1]));
            y_coord.push_back(std::stoi(tokens[2]));
        }
    }
    file.close();
}

