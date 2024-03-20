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
#include "matrix.h"
#include <sstream> 


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

    // Utils
    int find_random_start();
    int find_farthest(int);
    int calc_distance(int, int);
    std::pair<int, int> choose_starting_vertices();
    void append_vertex(int, std::vector<int>&);
    void insert_vertex(int, int, std::vector<int>&);
    void log_build_process();
    
    // Greedy
    auto find_greedy_cycles() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto find_greedy_cycles_expansion() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto find_greedy_regret_cycles() -> std::tuple<std::vector<int>, std::vector<int>>;

    // GreedyTSP specific functions
    std::vector<int> find_greedy_cycle();
    std::pair<int, double> find_nearest_expansion(int, int, const std::vector<bool>&);
    std::pair<int, int> find_nearest_neighbor(int, int, const std::vector<bool>&);
    std::pair<int, double> find_best_2regret(int, int, const std::vector<bool>&);

    // Local Search
    auto local_search() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto read_cycle(const std::string& file) -> std::vector<std::vector<int>>;

};

#endif // TSP_H
