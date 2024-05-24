#include "../lib/tsp.h"

auto TSP::find_greedy_cycles_nearest()
	-> std::tuple<std::vector<int>, std::vector<int>> {
	auto start = choose_starting_vertices();
	int current_last_vertex1 = start.first;
	int current_last_vertex2 = start.second;
	int current_first_vertex1 = start.first;
	int current_first_vertex2 = start.second;

	append_vertex(current_last_vertex1, cycle1);
	append_vertex(current_last_vertex2, cycle2);

	while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
		// Find nearest neighbors for the last and first vertices in each cycle
		auto nearest_neighbor_info1 = find_nearest_neighbor(
			current_last_vertex1, current_first_vertex1, visited);
		auto nearest_neighbor_info2 = find_nearest_neighbor(
			current_last_vertex2, current_first_vertex2, visited);

		int nearest_neighbor1 = nearest_neighbor_info1.first;
		int nearest_neighbor2 = nearest_neighbor_info2.first;
		int vertex_type1 = nearest_neighbor_info1.second;
		int vertex_type2 = nearest_neighbor_info2.second;

		std::vector<int> &cycle =
			(cycle1.size() <= cycle2.size()) ? cycle1 : cycle2;

		// Choose the nearest neighbor to the cycle that is shorter
		if (cycle1.size() <= cycle2.size()) {
			if (vertex_type1 == 0) {
				append_vertex(nearest_neighbor1, cycle);
				current_last_vertex1 = nearest_neighbor1;
			} else {
				insert_vertex(nearest_neighbor1, 0, cycle);
				current_first_vertex1 = nearest_neighbor1;
			}
		} else {
			if (vertex_type2 == 0) {
				append_vertex(nearest_neighbor2, cycle);
				current_last_vertex2 = nearest_neighbor2;
			} else {
				insert_vertex(nearest_neighbor2, 0, cycle);
				current_first_vertex2 = nearest_neighbor2;
			}
		}
	}
	return {cycle1, cycle2};
}

std::pair<int, int>
TSP::find_nearest_neighbor(int current_last_vertex, int current_first_vertex,
						   const std::vector<bool> &visited) {
	double min_distance_last = std::numeric_limits<double>::max();
	double min_distance_first = std::numeric_limits<double>::max();
	int nearest_neighbor_last = -1;
	int nearest_neighbor_first = -1;

	for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
		if (!visited[i] && i != current_last_vertex) {
			double distance = dist_matrix.dist_matrix[current_last_vertex][i];

			if (distance < min_distance_last) {
				min_distance_last = distance;
				nearest_neighbor_last = i;
			}
		}

		if (!visited[i] && i != current_first_vertex) {
			double distance = dist_matrix.dist_matrix[current_first_vertex][i];
			if (distance < min_distance_first) {
				min_distance_first = distance;
				nearest_neighbor_first = i;
			}
		}
	}

	if (min_distance_last < min_distance_first) {
		return {nearest_neighbor_last, 0}; // 0 - nearest for last vertex
	} else {
		return {nearest_neighbor_first, 1}; // 1 - nearest for first vertex
	}
}

auto TSP::find_greedy_cycles_expansion()
	-> std::tuple<std::vector<int>, std::vector<int>> {
	auto start = choose_starting_vertices();
	append_vertex(start.first, cycle1);
	append_vertex(start.second, cycle2);

	while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
		std::vector<int> &cycle =
			(cycle1.size() <= cycle2.size()) ? cycle1 : cycle2;

		double min_length = std::numeric_limits<double>::max();
		int best_candidate, best_idx = -1;

		for (size_t i = 0; i < cycle.size(); ++i) {
			int current = cycle[i];
			int next = cycle[(i + 1) % cycle.size()];
			auto [nearest, length] =
				find_nearest_expansion(current, next, visited);
			if (length < min_length) {
				min_length = length;
				best_candidate = nearest;
				best_idx = i;
			}
		}

		insert_vertex(best_candidate, (best_idx + 1) % cycle.size(), cycle);
	}
	return {cycle1, cycle2};
}

auto TSP::find_nearest_expansion(int first, int last,
								 const std::vector<bool> &visited)
	-> std::pair<int, double> {
	if (first == last) {
		return {find_nearest_neighbor(first, first, visited).first, 0.0};
	}

	double min_distance = std::numeric_limits<double>::max();
	int candidate = -1;

	for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
		if (!visited[i] && i != first && i != last) {
			double distance = get_expansion_cost(first, last, i);
			if (distance < min_distance) {
				min_distance = distance;
				candidate = i;
			}
		}
	}
	return {candidate, min_distance};
}

auto TSP::find_greedy_cycles_regret()
	-> std::tuple<std::vector<int>, std::vector<int>> {
	if (!cycle1.empty() && !cycle2.empty()) {
		cycle1.clear();
		cycle2.clear();
		visited = std::vector<bool>(dist_matrix.x_coord.size(), false);
	}
	auto start = choose_starting_vertices();
	append_vertex(start.first, cycle1);
	append_vertex(start.second, cycle2);
	std::map<int, std::pair<int, double>> regrets;

	while (cycle1.size() + cycle2.size() < dist_matrix.x_coord.size()) {
		std::vector<int> &cycle =
			(cycle1.size() <= cycle2.size()) ? cycle1 : cycle2;
		regrets.clear();
		for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
			if (!visited[i]) {
				regrets[i] = get_2regret(i, cycle);
			}
		}

		int candidate = -1;
		double max_regret = -std::numeric_limits<double>::infinity();
		for (const auto &r : regrets) {
			if (r.second.second > max_regret) {
				max_regret = r.second.second;
				candidate = r.first;
			}
		}
		insert_vertex(candidate, (regrets[candidate].first + 1) % cycle.size(),
					  cycle);
	}

	return {cycle1, cycle2};
}

auto TSP::get_2regret(int candidate, std::vector<int> cycle, float coef)
	-> std::pair<int, double> {
	std::vector<std::pair<int, double>> cost; // idx, distance
	for (size_t i = 0; i < cycle.size(); ++i) {
		double distance = get_expansion_cost(
			cycle[i], cycle[(i + 1) % cycle.size()], candidate);
		cost.push_back({i, distance});
	}
	// Find the indices of the two lowest values
	int min_idxs[2];
	min_idxs[0] = 0;
	min_idxs[1] = 1;
	if (cost[1].second < cost[min_idxs[0]].second) {
		min_idxs[0] = 1;
		min_idxs[1] = min_idxs[0];
	}

	for (size_t i = 2; i < cost.size(); ++i) {
		if (cost[i].second < cost[min_idxs[0]].second) {
			min_idxs[1] = min_idxs[0];
			min_idxs[0] = i;
		} else if (cost[i].second < cost[min_idxs[1]].second) {
			min_idxs[1] = i;
		}
	}

	double regret = cost[min_idxs[1]].second - cost[min_idxs[0]].second;
	double weighted_regret = regret - coef * cost[min_idxs[0]].second;

	return {cost[min_idxs[0]].first, weighted_regret};
}

double TSP::get_expansion_cost(int first, int last, int candidate) {
	return dist_matrix.dist_matrix[first][candidate] +
		   dist_matrix.dist_matrix[last][candidate] -
		   dist_matrix.dist_matrix[first][last];
}

auto TSP::find_greedy_cycles_regret_from_incomplete(std::vector<int> &c1,
													std::vector<int> &c2)
	-> std::tuple<std::vector<int>, std::vector<int>> {
	std::map<int, std::pair<int, double>> regrets;
	visited = std::vector<bool>(dist_matrix.x_coord.size(), false);
	for (int vertex : c1)
		visited[vertex] = true;
	for (int vertex : c2)
		visited[vertex] = true;
	while (c1.size() + c2.size() < dist_matrix.x_coord.size()) {
		std::vector<int> &cycle = (c1.size() <= c2.size()) ? c1 : c2;
		regrets.clear();
		for (size_t i = 0; i < dist_matrix.x_coord.size(); ++i) {
			if (!visited[i]) {
				regrets[i] = get_2regret(i, cycle);
			}
		}

		int candidate = -1;
		double max_regret = -std::numeric_limits<double>::infinity();
		for (const auto &r : regrets) {
			if (r.second.second > max_regret) {
				max_regret = r.second.second;
				candidate = r.first;
			}
		}
		insert_vertex(candidate, (regrets[candidate].first + 1) % cycle.size(),
					  cycle);
	}

	return {c1, c2};
}

auto TSP::find_greedy_cycles_from_incomplete(std::vector<int> &c1, std::vector<int> &c2) -> std::tuple<std::vector<int>, std::vector<int>> {
    // Greedy cycles expansion approach
    visited = std::vector<bool>(dist_matrix.x_coord.size(), false);
    for (int vertex : c1)
        visited[vertex] = true;
    for (int vertex : c2)
        visited[vertex] = true;
    while (c1.size() + c2.size() < dist_matrix.x_coord.size()) {
        std::vector<int> &cycle =
                (c1.size() <= c2.size()) ? c1 : c2;
        double min_length = std::numeric_limits<double>::max();
        int best_candidate, best_idx = -1;

        for (size_t i = 0; i < cycle.size(); ++i) {
            int current = cycle[i];
            int next = cycle[(i + 1) % cycle.size()];
            auto [nearest, length] =
                    find_nearest_expansion(current, next, visited);
            if (length < min_length) {
                min_length = length;
                best_candidate = nearest;
                best_idx = i;
            }
        }

        insert_vertex(best_candidate, (best_idx + 1) % cycle.size(), cycle);
    }

    return {c1, c2};
}

auto TSP::find_greedy_cycles_nearest_from_incomplete(std::vector<int> &c1, std::vector<int> &c2) -> std::tuple<std::vector<int>, std::vector<int>> {
    visited = std::vector<bool>(dist_matrix.x_coord.size(), false);
    for (int vertex : c1)
        visited[vertex] = true;
    for (int vertex : c2)
        visited[vertex] = true;

    int current_last_vertex1 = c1.back();
    int current_last_vertex2 = c2.back();
    int current_first_vertex1 = c1.front();
    int current_first_vertex2 = c2.front();

    while (c1.size() + c2.size() < dist_matrix.x_coord.size()) {
        auto nearest_neighbor_info1 = find_nearest_neighbor(
                current_last_vertex1, current_first_vertex1, visited);
        auto nearest_neighbor_info2 = find_nearest_neighbor(
                current_last_vertex2, current_first_vertex2, visited);

        int nearest_neighbor1 = nearest_neighbor_info1.first;
        int nearest_neighbor2 = nearest_neighbor_info2.first;
        int vertex_type1 = nearest_neighbor_info1.second;
        int vertex_type2 = nearest_neighbor_info2.second;

        std::vector<int> &cycle =
                (c1.size() <= c2.size()) ? c1 : c2;

        if (c1.size() <= c2.size()) {
            if (vertex_type1 == 0) {
                append_vertex(nearest_neighbor1, cycle);
                current_last_vertex1 = nearest_neighbor1;
            } else {
                insert_vertex(nearest_neighbor1, 0, cycle);
                current_first_vertex1 = nearest_neighbor1;
            }
        } else {
            if (vertex_type2 == 0) {
                append_vertex(nearest_neighbor2, cycle);
                current_last_vertex2 = nearest_neighbor2;
            } else {
                insert_vertex(nearest_neighbor2, 0, cycle);
                current_first_vertex2 = nearest_neighbor2;
            }
        }
    }
    return {c1, c2};
}