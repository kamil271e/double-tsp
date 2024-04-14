#include "../lib/utils.h"

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::vector<int>> generate_adj_matrix(const std::vector<int>& cycle){
    int n = cycle.size();
    std::vector<std::vector<int>> adj_matrix(2 * n, std::vector<int>(2 * n, 0));
    for (int i = 0; i < n; i++){
        adj_matrix[cycle[i]-1][cycle[(i+1)%n]-1] = 1;
        adj_matrix[cycle[(i+1)%n]-1][cycle[i]-1] = -1;
    }
    return adj_matrix;
}

void visualize_adj_matrix(std::vector<std::vector<int>> adj_matrix){
    for (int i = 0; i < adj_matrix.size(); ++i){
        for (int j = 0; j < adj_matrix.size(); ++j){
            std::cout << adj_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}