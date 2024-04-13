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

std::vector<std::vector<int>> generate_adj_matrix(std::vector<int> cycle){
    int n = cycle.size();
    std::vector<std::vector<int>> adj_matrix(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; i++){
        adj_matrix[cycle[i]][cycle[(i+1)%n]] = 1;
        adj_matrix[cycle[(i+1)%n]][cycle[i]] = -1;
        adj_matrix[cycle[(i+1)%n]][cycle[i]] = 1;
        adj_matrix[cycle[i]][cycle[(i+1)%n]] = -1;
    }
    return adj_matrix;
}