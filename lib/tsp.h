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
    std::vector<int> solve();

    const Matrix& dist_matrix;

    
private:
    std::vector<bool> visited = std::vector<bool>(dist_matrix.x_coord.size(), false);
    std::vector<int> cycle;
    AlgType alg_type;
    int find_random_start();
    int find_farthest(int);
    int find_nearest(int);
    int calc_distance(int, int);

    // GreedyTSP specific functions
    std::vector<int> find_greedy_cycle();
    int find_nearest_neighbor(int current_vertex, const std::vector<bool>& visited);
};

#endif // TSP_H
