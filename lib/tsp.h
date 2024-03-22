#ifndef TSP_H
#define TSP_H

#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>
#include <random>
#include <map>
#include <sstream>
#include <unordered_set>
#include <set>
#include <cstdlib> // for rand and srand
#include <ctime>   // for time
#include <algorithm> // for std::shuffle
#include <iomanip>
#include "matrix.h"


enum class AlgType {
    nearest_neighbors,
    greedy_cycle,
    regret,
    local,
};


class TSP{
public:
    TSP(const Matrix& dist_matrix, AlgType);
    auto solve() -> std::tuple<std::vector<int>, std::vector<int>>;
    const Matrix& dist_matrix;

private:
    std::vector<bool> visited = std::vector<bool>(dist_matrix.x_coord.size(), false);
    std::vector<int> cycle1;
    std::vector<int> cycle2;
    AlgType alg_type;

    // UTILS
    int find_random_start();
    int find_farthest(int);
    int calc_distance(int, int);
    double calc_cycle_len(const std::vector<int>&);
    std::pair<int, int> choose_starting_vertices();
    void append_vertex(int, std::vector<int>&);
    void insert_vertex(int, int, std::vector<int>&);
    void log_build_process();
    
    // GREEDY
    auto find_greedy_cycles() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto find_greedy_cycles_expansion() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto find_greedy_regret_cycles(float=0.42) -> std::tuple<std::vector<int>, std::vector<int>>;
    std::vector<int> find_greedy_cycle();
    std::pair<int, double> find_nearest_expansion(int, int, const std::vector<bool>&);
    std::pair<int, int> find_nearest_neighbor(int, int, const std::vector<bool>&);
    std::pair<int, double> find_best_2regret(int, int, const std::vector<bool>&);

    // LOCAL SEARCH
    auto local_search() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto hill_climbing(const std::vector<int>&, bool) -> std::vector<int>;
    auto read_cycle(const std::string&) -> std::vector<std::vector<int>>;
    auto generate_neighbors(const std::vector<int>&, int) -> std::vector<std::vector<int>>;
    auto generate_all_edge_movements(const std::vector<int>& ) -> std::vector<std::vector<int>>;
    auto generate_all_vertex_movements(const std::vector<int>& ) -> std::vector<std::vector<int>>;
    auto get_random_move(std::vector<std::vector<int>>) -> std::vector<int>;
    auto generate_random_cycles(int) -> std::tuple<std::vector<int>, std::vector<int>>;
    float get_objective_value(const std::vector<int>&, std::vector<int>);
};

#endif // TSP_H
