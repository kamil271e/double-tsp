#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void gen_coord_vectors(std::ifstream& file){
    std::vector<int> x_coord;
    std::vector<int> y_coord;
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
}

int main(int argc, char* argv[]){
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }
    gen_coord_vectors(file);
    file.close();
    return 0;
}
