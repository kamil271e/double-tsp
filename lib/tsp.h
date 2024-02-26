#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

enum class AlgType {
    nearest_neighbors,
    greedy_cycle,
    regret_heuristic
};

class TSP{
public:
    TSP(const std::vector<std::vector<int>>&, AlgType);
    void solve();
    const std::vector<std::vector<int>>& dist_matrix;
private:
    std::vector<bool> visited;
    std::vector<int> cycle;
    AlgType alg_type;
    int find_random_start();
    int find_farthest(int);
    int find_nearest(int);
    int calc_distance(int, int);
};
