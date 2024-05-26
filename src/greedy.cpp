#include "../lib/tsp.h"

auto TSP::find_greedy_cycles_nearest()
	-> std::tuple<std::vector<int>, std::vector<int>> {
    if (!cycle1.empty() && !cycle2.empty()) {
        cycle1.clear();
        cycle2.clear();
        visited = std::vector<bool>(dist_matrix.x_coord.size(), false);
    }
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

    auto handle_nearest_neighbor = [&](int &current_last_vertex, int &current_first_vertex, std::vector<int> &cycle) {
        auto nearest_neighbor_info = find_nearest_neighbor(current_last_vertex, current_first_vertex, visited);
        int nearest_neighbor = nearest_neighbor_info.first;
        int vertex_type = nearest_neighbor_info.second;

        if (vertex_type == 0) {
            append_vertex(nearest_neighbor, cycle);
            current_last_vertex = nearest_neighbor;
        } else {
            insert_vertex(nearest_neighbor, 0, cycle);
            current_first_vertex = nearest_neighbor;
        }
    };

    while (c1.size() + c2.size() < dist_matrix.x_coord.size()) {
        if (c1.size() <= c2.size()) {
            handle_nearest_neighbor(current_last_vertex1, current_first_vertex1, c1);
        } else {
            handle_nearest_neighbor(current_last_vertex2, current_first_vertex2, c2);
        }
    }
    return {c1, c2};
}

auto TSP::find_from_incomplete_degenerated(std::vector<std::vector<int>> &p1, std::vector<std::vector<int>> &p2,
                                           std::map<int, std::pair<int, int>> &m1, std::map<int, std::pair<int, int>> &m2) ->   std::tuple<std::vector<int>, std::vector<int>> {
    // Nearest neighbor approach
    // TODO
    return {p1.front(), p2.front()};
}

auto TSP::find_neighbour(int current_last_vertex, int current_first_vertex, int j) -> std::pair<int, int>{
    double min_distance_last = std::numeric_limits<double>::max();
    double min_distance_first = std::numeric_limits<double>::max();
    int nearest_neighbor_last = -1;
    int nearest_neighbor_first = -1;
    double distance_last = dist_matrix.dist_matrix[current_last_vertex][j];
    double distance_first = dist_matrix.dist_matrix[current_first_vertex][j];
    if (distance_last < min_distance_last){
        min_distance_last = distance_last;
        nearest_neighbor_last = j;
    }
    if (distance_first < min_distance_first){
        min_distance_first = distance_first;
        nearest_neighbor_first = j;
    }
    if (min_distance_last < min_distance_first){
        return {nearest_neighbor_last, 1};
    } else {
        return {nearest_neighbor_first, 0};
    }
}


auto TSP::find_from_incomplete_degenerated_inner(std::vector<std::vector<int>> &paths, std::map<int, std::pair<int, int>> &visited_map) -> std::vector<int> {
    int solution_idx = -1;

	// Iterate until the solution is found
    while(solution_idx == -1){

		// Iterate over all paths
        for (int i = 0; i < paths.size(); i++){

			// If the path is already complete, break the loop
            if (paths[i].size() >= 100){ // reconstruction finished
                solution_idx = i;
                break;
            }
            if (paths[i].empty()) continue; // empty path - ignore

			// Get the last and first vertex of the current path
            int current_last_vertex = paths[i].back();
            int current_first_vertex = paths[i].front();

			// Initialize the nearest neighbors and their distances
            double min_distance_last = std::numeric_limits<double>::max();
            double min_distance_first = std::numeric_limits<double>::max();
            int nearest_neighbor_last = -1;
            int nearest_neighbor_first = -1;

			// Iterate over all vertices to find the nearest neighbors for the last and first vertices
            for (int j = 0; j < dist_matrix.x_coord.size(); ++j) {
                // visited_map[vertex] = {free / not_available / path_idx, first/last}
				//If the vertex not in the current path and not visited(free)
                if (visited_map[j].first != i and visited_map[j].first != available(NOT)){

						// Get the distance between the last vertex and the current vertex, if it is smaller than the current minimum distance
                        double distance_last = dist_matrix.dist_matrix[current_last_vertex][j];
						if (distance_last < min_distance_last) {
                            min_distance_last = distance_last;
                            nearest_neighbor_last = j;
                    
						// Get the distance between the first vertex and the current vertex, if it is smaller than the current minimum distance
                        double distance_first = dist_matrix.dist_matrix[current_first_vertex][j];
                        if (distance_first < min_distance_first) {
                            min_distance_first = distance_first;
                            nearest_neighbor_first = j;
                        }
                    }
                }
            }

			//Checkpoint. What we have?
			// nearest_neighbor_last, nearest_neighbor_first - the nearest neighbors for the last and first vertices
			// min_distance_last, min_distance_first - the distances to the nearest neighbors
			//-----------------------------------------------------------------------------------------------------

			// Choose the best vertex to add to the path
            int best_vertex, which_side;
			// If the distance from the last vertex is smaller than the distance from the first vertex, choose the last vertex and set the side to RIGHT
            if (min_distance_last < min_distance_first){
                best_vertex = nearest_neighbor_last;
                which_side = side(RIGHT);
            } else {
				// Otherwise, choose the first vertex and set the side to LEFT
                best_vertex = nearest_neighbor_first;
                which_side = side(LEFT);
            }

			//Checkpoint. What we have?
			// best_vertex - the best vertex to add to the path
			// which_side - the side to add the vertex to
			//-----------------------------------------------------------------------------------------------------
			
            int j = best_vertex; // change the name for better readability

			// Print the best vertex
			std::cout << "Current right vertex: " << current_last_vertex << std::endl;
			std::cout << "Current left vertex: " << current_first_vertex << std::endl;
			std::cout << "Neigbour Vertex: " << j << " Index in paths list: " << visited_map[j].first << std::endl;
			std::cout << "Which Side(1 = Right| -1 = Left): " << which_side << std::endl;
			std::cout << std::endl;


			//Print the current path
			std::cout << "Current Path: ";	
			for (int vertex : paths[i]){
				std::cout << vertex << " ";
			}
			std::cout << std::endl;

			// print the  paths[visited_map[j].first]
			// std::cout << "Neighbour path: ";
			// for (int vertex : paths[visited_map[j].first]){
			// 	std::cout << vertex << " ";
			// }
			// std::cout << std::endl;

			// If the vertex is isolated, add it to the path
            if (visited_map[j].first == available(FREE)){ // isolated vertex
                // auto [vertex, where] = find_neighbour(current_last_vertex, current_first_vertex, j);
				std::cout << "FREE" << std::endl;

				// If the vertex is on the right side, add it to the end of the path
                if (which_side == side(RIGHT)){
                    visited_map[current_last_vertex] = {available(NOT), 0};
                    visited_map[j] = {i, side(RIGHT)};
                    paths[i].push_back(j);

				// Otherwise, add it to the beginning of the path and 
                }else { // left
					// Change current_first_vertex status to NOT available
                    visited_map[current_first_vertex] = {available(NOT), 0};
					// Change the status of the neighbour vertex to the path index and the side to LEFT
                    visited_map[j] = {i, side(LEFT)};
					// Add the vertex to the beginning of the path
                    paths[i].insert(paths[i].begin(), j);
                }

			// If the vertex is already in a path
            }else{

				// If the vertex is on the right side
                if (visited_map[j].second == side(RIGHT)){ // merge paths from the RIGHT; j from RIGHT
					visited_map[j] = {available(NOT), 0}; // change status of the vertex to NOT available

                    // TODO j-ty z prawej strony:
					if (which_side  == side(RIGHT)) { // Unusual case, because the vertex is on the right side 
						std::cout << "i RIGHT j RIGHT " << std::endl;

                        // j from RIGHT and i from RIGHT: paths[i] = merged(paths[i], std::reversed(paths[j]))
						visited_map[current_last_vertex] = {available(NOT), 0}; // change status of the current_last_vertex to NOT available, because it will be merged with the path from the end


						// Reverse the path and merge it with the current path
                        std::reverse(paths[visited_map[j].first].begin(), paths[visited_map[j].first].end()); //????
                        paths[i].insert(paths[i].begin(), paths[visited_map[j].first].begin(),
                                        paths[visited_map[j].first].end());

						// Update the visited_map
                        visited_map[paths[i].back()] = {i, side(RIGHT)};
						visited_map[j] = {available(NOT), 0}; // change status of the vertex to NOT available
                        paths[visited_map[j].first].clear();

                    } else { // left		
						std::cout << "i LEFT j RIGHT " << std::endl;

                        // j from RIGHT i from LEFT: paths[i] = merged(paths[j], paths[i])
                        visited_map[current_first_vertex] = {available(NOT), 0};

                        //visited_map[visited_map[j].first] = {available(NOT), 0};// ???
                        
						std::vector<int> tmp = paths[i];
                        paths[i].clear();
                        paths[i].insert(paths[i].end(), paths[visited_map[j].first].begin(), paths[visited_map[j].first].end());
                        paths[i].insert(paths[i].end(), tmp.begin(), tmp.end()); 


						visited_map[paths[i].front()] = {i, side(LEFT)};

						visited_map[j] = {available(NOT), 0}; // change status of the vertex to NOT available
                        paths[visited_map[j].first].clear();
                    }
                } else { // merge paths to the LEFT


					// TODO j-ty z lewej strony:
                    if (which_side == side(RIGHT)){
						std::cout << "i RIGHT j LEFT " << std::endl;

						// Print the  neighbour path
						std::cout << "Neighbour path: ";
						for (int vertex : paths[visited_map[j].first]){
							std::cout << vertex << " ";
						}
						std::cout << std::endl;

                        // j from LEFT i from RIGHT: paths[i] = merged(paths[i], paths[j])
                        visited_map[current_last_vertex] = {available(NOT), 0};

                       
                        paths[i].insert(paths[i].end(), paths[visited_map[j].first].begin(), paths[visited_map[j].first].end());						
						std::cout << "I'm here!" << std::endl;

						visited_map[paths[i].back()] = {i, side(RIGHT)};
						
						visited_map[j] = {available(NOT), 0};
                        paths[visited_map[j].first].clear();
						
                    } else { // left
						std::cout << "i LEFT j LEFT " << std::endl;

                        // j from LEFT i from LEFT: paths[i] = merged(std::reversed(paths[j]), paths[i])
                        visited_map[current_first_vertex] = {available(NOT), 0};
                        //visited_map[visited_map[j].first] = {available(NOT), 0};

                        std::reverse(paths[visited_map[j].first].begin(), paths[visited_map[j].first].end());
                        std::vector<int> tmp = paths[i];
                        paths[i].clear();
                        paths[i].insert(paths[i].begin(), paths[visited_map[j].first].begin(), paths[visited_map[j].first].end());
                        paths[i].insert(paths[i].end(), tmp.begin(), tmp.end());

                        visited_map[paths[i].front()] = {i, side(LEFT)};

						visited_map[j] = {available(NOT), 0};
                        paths[visited_map[j].first].clear();
                    }
                }
            }

			//Print the current path
			std::cout << "New Path: ";
			for (int vertex : paths[i]){
				std::cout << vertex << " ";
			}
			std::cout << std::endl;
			std::cout << "----------------------------------------------" <<std::endl;

        }
    }

    return paths[solution_idx];
}


