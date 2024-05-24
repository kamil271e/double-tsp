#include <utility>

#include "../lib/tsp.h"

auto TSP::generate_all_edge_movements(int n) -> std::vector<std::vector<int>> {
	// idx:2 inner move : type = 0
	// idx:2 inter movement: type = 1
	// idx:3 edge = 0, vertex = 1
	// idx:4 cycle1 = 0, cycle2 = 1
	std::vector<std::vector<int>> movements; // i, j, type
	for (int i = 0; i < n; ++i) {
		for (int j = i + 2; j < n; ++j) {
			if (i == 0 && j == n - 1)
				continue;
			for (int k = 0; k < 2; ++k) {
				movements.push_back({i, j, 0, 0, k});
			}
		}
	}
	return movements;
}

auto TSP::generate_all_vertex_movements(int n)
	-> std::vector<std::vector<int>> {
	// inner move : type = 0
	// inter movement: type = 1
	std::vector<std::vector<int>> movements; // i, j, type
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			if (i == 0 && j == n - 1)
				continue; // redundant - edge move (1, n-2) is the same as
						  // vertex move (0, n-1)
			for (int k = 0; k < 2; ++k) {
				movements.push_back({i, j, 0, 0, k});
			}
		}
	}
	return movements;
}

auto TSP::generate_all_vertex_movements_inter(int n)
	-> std::vector<std::vector<int>> {
	// inner move : type = 0
	// inter movement: type = 1
	std::vector<std::vector<int>> movements;
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (i != j)
				movements.push_back({i, j, 1, 1});
		}
	}
	return movements;
}

auto TSP::get_delta(std::vector<int> movement) -> std::tuple<int, int> {
	int i = movement[0];
	int j = movement[1];
	int n = cycle1.size();
	int i_left = (i - 1 + n) % n;
	int i_right = (i + 1) % n;
	int j_left = (j - 1 + n) % n;
	int j_right = (j + 1) % n;
	float deleted, added;
	int cycle_chosen = -1; // -1 none; 0 cycle1; 1 cycle2

	if (movement[2] == 0) { // inner move
		cycle_chosen = movement[4];
		std::vector<int> &cycle =
			(cycle_chosen == 0) ? cycle1 : cycle2; // TODO: potential issue
		if (movement[3] == 0) {					   // edge
			deleted = dist_matrix.dist_matrix[cycle[i]][cycle[i_left]] +
					  dist_matrix.dist_matrix[cycle[j]][cycle[j_right]];
			added = dist_matrix.dist_matrix[cycle[i]][cycle[j_right]] +
					dist_matrix.dist_matrix[cycle[i_left]][cycle[j]];
		} else {						// vertex
			if (i == 0 && j == n - 1) { // extreme case
				deleted = dist_matrix.dist_matrix[cycle[i]][cycle[i_right]] +
						  dist_matrix.dist_matrix[cycle[j]][cycle[j_left]];
				added = dist_matrix.dist_matrix[cycle[i]][cycle[j_left]] +
						dist_matrix.dist_matrix[cycle[j]][cycle[i_right]];
			} else if (j - i == 1) {
				deleted = dist_matrix.dist_matrix[cycle[i]][cycle[i_left]] +
						  dist_matrix.dist_matrix[cycle[j]][cycle[j_right]];
				added = dist_matrix.dist_matrix[cycle[i]][cycle[j_right]] +
						dist_matrix.dist_matrix[cycle[i_left]][cycle[j]];
			} else {
				deleted = dist_matrix.dist_matrix[cycle[i]][cycle[i_left]] +
						  dist_matrix.dist_matrix[cycle[i]][cycle[i_right]] +
						  dist_matrix.dist_matrix[cycle[j]][cycle[j_left]] +
						  dist_matrix.dist_matrix[cycle[j]][cycle[j_right]];
				added = dist_matrix.dist_matrix[cycle[i]][cycle[j_left]] +
						dist_matrix.dist_matrix[cycle[i]][cycle[j_right]] +
						dist_matrix.dist_matrix[cycle[j]][cycle[i_left]] +
						dist_matrix.dist_matrix[cycle[j]][cycle[i_right]];
			}
		}
	} else { // inter move
		deleted = dist_matrix.dist_matrix[cycle1[i]][cycle1[i_left]] +
				  dist_matrix.dist_matrix[cycle1[i]][cycle1[i_right]] +
				  dist_matrix.dist_matrix[cycle2[j]][cycle2[j_left]] +
				  dist_matrix.dist_matrix[cycle2[j]][cycle2[j_right]];
		added = dist_matrix.dist_matrix[cycle1[i]][cycle2[j_left]] +
				dist_matrix.dist_matrix[cycle1[i]][cycle2[j_right]] +
				dist_matrix.dist_matrix[cycle2[j]][cycle1[i_left]] +
				dist_matrix.dist_matrix[cycle2[j]][cycle1[i_right]];
	}
	return {(int)(deleted - added), cycle_chosen};
}

void TSP::update_cycle(const std::vector<int> &movement,
					   std::vector<int> &cycle) {
	int i = movement[0];
	int j = movement[1];
	int type = movement[3];
	if (type == 0) { // edge
		std::reverse(cycle.begin() + i, cycle.begin() + j + 1);
	} else { // vertex
		std::swap(cycle[i], cycle[j]);
	}
}

void TSP::apply_movement(const std::vector<int> &movement, int cycle_num) {
	if (cycle_num == 0) {
		update_cycle(movement, cycle1);
	} else if (cycle_num == 1) {
		update_cycle(movement, cycle2);
	} else {
		update_cycles(movement);
	}
}

void TSP::update_cycles(std::vector<int> movement) {
	int i = movement[0]; // idx of vertex in cycle1
	int j = movement[1]; // idx of vertex in cycle2
	int temp = cycle1[i];
	cycle1[i] = cycle2[j];
	cycle2[j] = temp;
}

void TSP::main_search(bool steepest, bool vertex) {
	std::vector<std::vector<int>> movements;
	std::vector<std::vector<int>> movements_inner;
	std::vector<std::vector<int>> movements_inter =
		generate_all_vertex_movements_inter(cycle1.size());
	if (vertex) {
		movements_inner = generate_all_vertex_movements(cycle1.size());
	} else {
		movements_inner = generate_all_edge_movements(cycle1.size());
	}
	movements.insert(movements.end(), movements_inter.begin(),
					 movements_inter.end());
	movements.insert(movements.end(), movements_inner.begin(),
					 movements_inner.end());

	std::vector<int> best_movement;
	bool found_better;
	int objective_value, best_objective_value;
	int cycle_num, best_cycle_num;

	do {
		best_objective_value = 0;
		best_cycle_num = -1;
		found_better = false;
		std::shuffle(movements.begin(), movements.end(),
					 std::mt19937(std::random_device()())); // shuffle movements
		for (int iter = 0; iter < movements.size(); ++iter) {
			std::tie(objective_value, cycle_num) =
				get_delta(movements[iter]); // we need to remember chosen cycle
											// for apply_movement method; type =
											// 0 - cycle1, 1 - cycle2
			if (objective_value > 0) { // better than current
				//                std::cout << movements[iter][0] << " " <<
				//                movements[iter][1] << " " <<
				//                movements[iter][2] << " "
				//                << cycle_num << std::endl;
				found_better = true;
				if (steepest) {
					if (objective_value > best_objective_value) {
						best_objective_value = objective_value;
						best_movement = movements[iter];
						best_cycle_num = cycle_num;
					}
				} else {
					apply_movement(movements[iter], cycle_num);
					found_better = false;
					break;
				}
			}
		}
		if (steepest && found_better) {
			apply_movement(best_movement, best_cycle_num);
		}
	} while (found_better);
}

// Function to perform local search
auto TSP::local_search() -> std::tuple<std::vector<int>, std::vector<int>> {
	// Types of input data for the cycles generation
	if (params.input_data == "random") {
		std::tie(cycle1, cycle2) = generate_random_cycles(100);

	} else if (params.input_data == "regret") {
		std::tie(cycle1, cycle2) = find_greedy_cycles_regret();
	}

	bool vertex;
	if (params.movements_type == "edge") {
		vertex = false;
	} else {
		vertex = true;
	}
	// auto start_time_local = std::chrono::high_resolution_clock::now();

	main_search(params.steepest, vertex);

	// auto end_time_local = std::chrono::high_resolution_clock::now();
	// auto duration_local =
	// std::chrono::duration_cast<std::chrono::milliseconds>(end_time_local -
	// start_time_local).count(); save_time(duration_local, params, "local");

	return {cycle1, cycle2};
}

auto TSP::local_search(std::vector<int> c1, std::vector<int> c2)
	-> std::tuple<std::vector<int>, std::vector<int>> {
	cycle1 = std::move(c1);
	cycle2 = std::move(c2);
	bool vertex;
	if (params.movements_type == "edge") {
		vertex = false;
	} else {
		vertex = true;
	}

	main_search(params.steepest, vertex);

	return {cycle1, cycle2};
}
