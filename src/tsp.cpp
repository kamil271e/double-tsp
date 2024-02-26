#include "../lib/tsp.h"

TSP::TSP(const std::vector<std::vector<int>>& dist_matrix, AlgType alg_type){
    this->dist_matrix = dist_matrix;
    this->alg_type = alg_type;
}

void TSP::solve(){}

int TSP::find_random_start(){}

int TSP::find_farthest(int node){}

int TSP::find_nearest(int node){}

int TSP::calc_distance(int node1, int node2){}

