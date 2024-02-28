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
    int find_nearest(int);
    int calc_distance(int, int);
    std::pair<int, int> choose_starting_vertices();
    auto find_greedy_cycles() -> std::tuple<std::vector<int>, std::vector<int>>;
    // GreedyTSP specific functions
    std::vector<int> find_greedy_cycle();
    int find_nearest_neighbor(int current_vertex, const std::vector<bool>& visited);
};

#endif // TSP_H
