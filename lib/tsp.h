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
#include "matrix.h"

enum class AlgType {
    nearest_neighbors,
    greedy_cycle,
    regret_heuristic
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

    int find_random_start();
    int find_farthest(int);
    int calc_distance(int, int);
    std::pair<int, int> choose_starting_vertices();
    void add_vertex(int);
    void log_build_process();
    
    auto find_greedy_cycles() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto find_greedy_cycles_expansion() -> std::tuple<std::vector<int>, std::vector<int>>;
    
    // GreedyTSP specific functions
    std::vector<int> find_greedy_cycle();
    int find_nearest_neighbor(int, const std::vector<bool>&);
    std::pair<int, double> find_nearest_expansion(int, int, const std::vector<bool>&);
};

#endif // TSP_H
