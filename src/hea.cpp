#include "../lib/tsp.h"

auto TSP::hybrid_evolution_algo()
	-> std::tuple<std::vector<int>, std::vector<int>> {
	// Initialization
	// Generate an initial population of 20 solutions using local search
	// methods.
	bool using_local_search = true;
	std::vector<std::tuple<std::vector<int>, std::vector<int>>> population;
	for (int i = 0; i < 20; i++) {
		auto [c1, c2] = local_search();
		population.push_back({c1, c2});
	}

	// Ensure no duplicate solutions exist in the population.
	std::sort(population.begin(), population.end());
	population.erase(std::unique(population.begin(), population.end()),
					 population.end());

	auto avg_time = calculateAverageMSLStime();

	// TODO: put it in separate function:

    // while (time < avg_time): ...

        auto [parent1, parent2] = select_two_parents(population);
        auto& [parent1_cycle1, parent1_cycle2] = population[parent1];
        auto& [parent2_cycle1, parent2_cycle2] = population[parent2];

        auto edges1 = findEdges(parent2_cycle1);
        auto edges2 = findEdges(parent2_cycle2);
        std::unordered_set<std::pair<int, int>, pair_hash> edges;
        edges.insert(edges1.begin(), edges1.end());
        edges.insert(edges2.begin(), edges2.end());

        std::map<int, std::pair<int, int>> visited_map1;
        std::map<int, std::pair<int, int>> visited_map2;
        auto paths1 = remove_edges(parent1_cycle1, edges, visited_map1); // czy przy tej logice 2 roznie visited_map potrzebne
        auto paths2 = remove_edges(parent1_cycle2, edges, visited_map2);

        // TODO implement reconstruction:
        // Narazie bez ruchów między klasowych

        auto c1 = find_from_incomplete_degenerated_inner(paths1, visited_map1);
        auto c2 = find_from_incomplete_degenerated_inner(paths2, visited_map2);
        auto solution = {c1, c2};

        // solution = reconstruction(paths1, paths2, visited_map1, visited_map2);
        // if worst(population) < current solution -> replace worst with current solution

    // end while;

    // return best_solution;

    return {cycle1, cycle2};
}

// Select two different parent solutions uniformly at random.
auto TSP::select_two_parents(
	const std::vector<std::tuple<std::vector<int>, std::vector<int>>>
		population) -> std::pair<int, int> {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, population.size() - 1);

	int parent1 = dist(gen);
	int parent2 = dist(gen);
	while (parent1 == parent2) {
		parent2 = dist(gen);
	}

	return std::make_pair(parent1, parent2);
}

// Helper function to find edges in a cycle
std::unordered_set<std::pair<int, int>, pair_hash>
TSP::findEdges(const std::vector<int> &cycle) {
	std::unordered_set<std::pair<int, int>, pair_hash> edges;
	for (size_t i = 0; i < cycle.size(); ++i) {
		int from = cycle[i];
		int to = cycle[(i + 1) % cycle.size()];
		if (from > to)
			std::swap(from, to); // Ensure consistent ordering // TODO: not needed - we do double checks in remove edges: {a,b} and {b,a}
		edges.emplace(from, to);
	}
	return edges;
}

auto remove_edges(std::vector<int>& cycle, const std::unordered_set<std::pair<int, int>, pair_hash>& other_edges, std::map<int, std::pair<int, int>>& visited_map) -> std::vector<std::vector<int>> {
    std::vector<std::vector<int>> paths;
    std::vector<int> current_path;
    // visited_map[vertex] = {free / not_available / path_idx, first/last}
    bool new_path = true;
    for (size_t i = 0; i < cycle.size(); ++i) {
        int from = cycle[i];
        int after = cycle[(i + 1) % cycle.size()];
        if ( (other_edges.find({from, after}) == other_edges.end() && other_edges.find({after, from}) == other_edges.end())) {
            current_path.push_back(from);
            if (current_path.size() == 1){
                // check if it has connection with previous
                // if so add it as a one-element path: at the end it will be merged with the path from the end
                // otherwise set is as a isolated vertex
                int previous = cycle[(i - 1 + cycle.size()) % cycle.size()];
                if (other_edges.find({previous, from}) == other_edges.end() && other_edges.find({from, previous}) == other_edges.end()){ // no edge
                    visited_map[from] = {available(FREE), 0}; // isolated vertex

                }else{
                    visited_map[from] = {paths.size(), side(RIGHT)}; // right element of path - available
                    paths.push_back(current_path);
                }

            } else{
                visited_map[from] = {paths.size(), side(RIGHT)}; // right element of path - available
                paths.push_back(current_path);
            }
            current_path.clear();
            new_path = true;
        }
        else { // edge {from, after} exists
            current_path.push_back(from);
            if (new_path){
                visited_map[from] = {paths.size(), side(LEFT)};
                new_path = false;
            } else{
                visited_map[from] = {available(NOT), 0}; // regular element inside a path - not left nor right: initialized as not available
            }
        }
    }
    // One more check - if last element does have an edge with the first element - add current path to first path; and adjust visited_map accordingly
    if (other_edges.find({cycle.back(), cycle.front()}) != other_edges.end() || other_edges.find({cycle.front(), cycle.back()}) != other_edges.end()) {
        visited_map[paths[0].front()] = {available(NOT), 0}; // first element of old first path now would be somewhere in the middle - not available
        paths[0].insert(paths[0].begin(), current_path.begin(), current_path.end());
        visited_map[paths[0].front()] = {0, side(LEFT)};
        visited_map[paths[0].back()] = {0, side(RIGHT)};
    }
    else {
        if (!current_path.empty()){
            visited_map[current_path.back()] = {paths.size(), side(RIGHT)};
            paths.push_back(current_path);
        }
    }
    return paths;
}