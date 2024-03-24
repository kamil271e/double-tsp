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
#include <chrono> 
#include "matrix.h"


enum class AlgType {
    nearest_neighbors,
    greedy_cycle,
    regret,
    local,
};

struct LocalSearchParams {
    std::string input_data; // random, regret
    std::string movements_type; // inner, inter
    std::string filename; // name of the file
    int steepest; // greedy(0), steepest(1)
};

class TSP{
public:
    TSP(const Matrix& dist_matrix, AlgType); // regular constructor
    TSP(const Matrix& dist_matrix, AlgType, std::string input_data, std::string movements_type, int steepest, std::string filename); // local search constructor

    auto solve() -> std::tuple<std::vector<int>, std::vector<int>>;
    const Matrix& dist_matrix;
    int start_idx=0;

private:
    std::vector<bool> visited = std::vector<bool>(dist_matrix.x_coord.size(), false);
    std::vector<int> cycle1, cycle2;
    AlgType alg_type;
    LocalSearchParams params;

    // UTILS
    int find_random_start();
    int find_farthest(int);
    int calc_distance(int, int);
    double calc_cycles_len();
    std::pair<int, int> choose_starting_vertices();
    void append_vertex(int, std::vector<int>&);
    void insert_vertex(int, int, std::vector<int>&);
    void log_build_process();
    void save_time_and_length(long, struct LocalSearchParams);
    
    // GREEDY
    auto find_greedy_cycles_nearest() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto find_greedy_cycles_expansion() -> std::tuple<std::vector<int>, std::vector<int>>;
    auto find_greedy_cycles_regret() -> std::tuple<std::vector<int>, std::vector<int>>;
    std::pair<int, double> find_nearest_expansion(int, int, const std::vector<bool>&);
    double get_expansion_cost(int, int, int);
    std::pair<int, int> find_nearest_neighbor(int, int, const std::vector<bool>&);
    std::pair<int, double> get_2regret(int, std::vector<int>, float=0.42);

    // LOCAL SEARCH
    auto local_search() -> std::tuple<std::vector<int>, std::vector<int>>;
    void inner_class_search(std::vector<int>&, bool);
    void inter_class_search(bool);
    auto generate_all_edge_movements(int) -> std::vector<std::vector<int>>;
    auto generate_all_vertex_movements(int) -> std::vector<std::vector<int>>;
    auto generate_all_vertex_movements_inter(int) -> std::vector<std::vector<int>>;
    auto generate_random_cycles(int) -> std::tuple<std::vector<int>, std::vector<int>>;
    int get_objective_value(const std::vector<int>&, std::vector<int>);                 // inner class
    int get_objective_value(std::vector<int>);                                          // inter class
    void update_cycle(const std::vector<int>&, std::vector<int>&);                      // inner class
    void update_cycles(std::vector<int>);                                               // inter class
};

#endif // TSP_H
