#ifndef TSP_H
#define TSP_H

#pragma once
#include "matrix.h"
#include <algorithm>
#include <algorithm> // for std::shuffle
#include <chrono>
#include <cmath>
#include <cstdlib> // for rand and srand
#include <ctime>   // for time
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

enum class AlgType {
	nearest_neighbors,
	greedy_cycle,
	regret,
	local,
	random_walk,
	multiple_local_search,
	ils1,
	ils2,
	hea
};

enum available {
    NOT=-2,
    FREE=-1,
    // other number is current path idx
};

enum side {
    LEFT=-1,
    RIGHT=1
};

struct LocalSearchParams {
	std::string input_data;		// random, regret
	std::string movements_type; // vertex, edge
	std::string filename;		// name of the file
	int steepest;				// greedy(0), steepest(1)
	int num_starts = 100;		// number of starts for multiple local search
};

// Hash function for pairs (to use pair as key in unordered_set)
struct pair_hash {
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2> &pair) const {
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};

class TSP {
  public:
	TSP(const Matrix &dist_matrix, AlgType); // regular constructor
	TSP(const Matrix &dist_matrix, AlgType, std::string input_data,
		std::string movements_type, int steepest,
		std::string filename); // local search constructor

	auto solve() -> std::tuple<std::vector<int>, std::vector<int>>;
	const Matrix &dist_matrix;
	int start_idx = -1; // random start

  private:
	std::vector<bool> visited =
		std::vector<bool>(dist_matrix.x_coord.size(), false);
	std::vector<int> cycle1, cycle2;
	AlgType alg_type;
	LocalSearchParams params;

	// UTILS
	int find_random_start();
	int find_farthest(int);
	int calc_distance(int, int);
	double calc_cycles_len();
	std::pair<int, int> choose_starting_vertices();
	void append_vertex(int, std::vector<int> &);
	void insert_vertex(int, int, std::vector<int> &);
	void log_build_process();
	void save_time(long, struct LocalSearchParams,
				   std::string); // TODO make more generic ;-;

	// GREEDY
	auto find_greedy_cycles_nearest()
		-> std::tuple<std::vector<int>, std::vector<int>>;
	auto find_greedy_cycles_expansion()
		-> std::tuple<std::vector<int>, std::vector<int>>;
	auto find_greedy_cycles_regret()
		-> std::tuple<std::vector<int>, std::vector<int>>;
	std::pair<int, double> find_nearest_expansion(int, int,
												  const std::vector<bool> &);
	double get_expansion_cost(int, int, int);
	std::pair<int, int> find_nearest_neighbor(int, int,
											  const std::vector<bool> &);
	std::pair<int, double> get_2regret(int, std::vector<int>, float = 0.1);
	std::tuple<std::vector<int>, std::vector<int>>
	find_greedy_cycles_regret_from_incomplete(std::vector<int> &,
											  std::vector<int> &);
    std::tuple<std::vector<int>, std::vector<int>> find_greedy_cycles_from_incomplete(std::vector<int> &, std::vector<int> &);
    std::tuple<std::vector<int>, std::vector<int>> find_greedy_cycles_nearest_from_incomplete(std::vector<int> &, std::vector<int> &);
    std::vector<int> find_from_incomplete_degenerated_inner(std::vector<std::vector<int>> &, std::map<int, std::pair<int, int>>&);
    std::tuple<std::vector<int>, std::vector<int>> find_from_incomplete_degenerated(std::vector<std::vector<int>> &, std::vector<std::vector<int>> &, std::map<int, std::pair<int, int>>&, std::map<int, std::pair<int, int>>&);
    std::pair<int,int> find_neighbour(int, int, int);

	// LOCAL SEARCH
	auto local_search() -> std::tuple<std::vector<int>, std::vector<int>>;
	auto local_search(std::vector<int>, std::vector<int>)
		-> std::tuple<std::vector<int>, std::vector<int>>;

	// auto random_walk() -> std::tuple<std::vector<int>, std::vector<int>>;
	// void inner_class_search(std::vector<int>&, bool);
	// void inter_class_search(bool);
	static auto generate_all_edge_movements(int)
		-> std::vector<std::vector<int>>;
	static auto generate_all_vertex_movements(int)
		-> std::vector<std::vector<int>>;
	static auto generate_all_vertex_movements_inter(int)
		-> std::vector<std::vector<int>>;
	auto generate_random_cycles(int)
		-> std::tuple<std::vector<int>, std::vector<int>>;
	// int get_objective_value(const std::vector<int>&, std::vector<int>); //
	// inner class int get_objective_value(std::vector<int>); // inter class
	void update_cycle(const std::vector<int> &,
					  std::vector<int> &); // inner class
	void update_cycles(std::vector<int>);  // inter class
	// void random_walk_inner(std::vector<int>, int) ;
	// void random_walk_inter(int);
	void apply_movement(const std::vector<int> &, int);
	void main_search(bool, bool);
	auto get_delta(std::vector<int> movement) -> std::tuple<int, int>;

	// LOCAL SEARCH EXTENSION
	auto multiple_local_search()
		-> std::tuple<std::vector<int>, std::vector<int>>;
	int calculate_objective(const std::vector<int> &, const std::vector<int> &);
	// Temporary solution to the problem of duplicate vertices in the cycle
	auto iterative_local_search_one()
		-> std::tuple<std::vector<int>, std::vector<int>>;
	auto iterative_local_search_two()
		-> std::tuple<std::vector<int>, std::vector<int>>;
	auto perturbation_one(std::vector<int> &, std::vector<int> &)
		-> std::tuple<std::vector<int>, std::vector<int>>;
	std::chrono::steady_clock::time_point calculateAverageMSLStime();

	auto destroy_perturbation(std::vector<int> &, std::vector<int> &)
		-> std::tuple<std::vector<int>, std::vector<int>>;
	auto repair_perturbation(std::vector<int> &, std::vector<int> &)
		-> std::tuple<std::vector<int>, std::vector<int>>;

	// HEA
	auto hybrid_evolution_algo()
		-> std::tuple<std::vector<int>, std::vector<int>>;
	auto select_two_parents(
		const std::vector<std::tuple<std::vector<int>, std::vector<int>>>)
		-> std::pair<int, int>;

	std::unordered_set<std::pair<int, int>, pair_hash>
	findEdges(const std::vector<int> &);
	
    auto remove_edges(std::vector<int>& , const std::unordered_set<std::pair<int, int>, pair_hash>&) -> std::vector<int>;
	auto find_best_worst_solution(std::vector<std::tuple<std::vector<int>, std::vector<int>>>, bool) -> std::pair<int, int>;

};
#endif // TSP_H
