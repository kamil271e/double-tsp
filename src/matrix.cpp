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
    while (std::getline(file, line) && line != "EOF") {
        if (line == "NODE_COORD_SECTION") {
            reading_coords = true;
            continue;
        }
        if (!reading_coords) {
            continue;
        }
        std::vector<std::string> tokens = splitString(line, ' ');
        if (tokens.size() >= 3) {
            this->x_coord.push_back(std::stoi(tokens[1]));
            this->y_coord.push_back(std::stoi(tokens[2]));
        }
    }
    file.close();
}

void Matrix::generate_dist_matrix(){
    for (int i = 0; i < this->x_coord.size(); i++){
        std::vector<int> row;
        for (int j = 0; j < this->x_coord.size(); j++){
            int dist = this->calc_distance(this->x_coord[i], this->y_coord[i],
                                            this->x_coord[j], this->y_coord[j]);
            row.push_back(dist);        
        }
        this->dist_matrix.push_back(row);
    }
}

int Matrix::calc_distance(int x1, int y1, int x2, int y2){
    return round(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
}
