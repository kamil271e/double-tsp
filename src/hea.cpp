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

        // inside remove_edges() we need to fill visited_map properly:
        // visited_map[vertex] = {free/path_idx, first/last}

        std::map<int, std::pair<int, int>> visited_map;
        auto paths1 = remove_edges(parent1_cycle1, edges, visited_map);
        auto paths2 = remove_edges(parent1_cycle2, edges, visited_map);

        // TODO implement reconstruction:
        // solution = reconstruction(paths1, paths2, visited_map);

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
			std::swap(from, to); // Ensure consistent ordering
		edges.emplace(from, to);
	}
	return edges;
}

auto TSP::remove_edges(std::vector<int>& cycle, const std::unordered_set<std::pair<int, int>, pair_hash>& other_edges, std::map<int, std::pair<int, int>>& visited_map) -> std::vector<std::vector<int>> {
    std::vector<std::vector<int>> paths;
    std::vector<int> current_path;

    for (size_t i = 0; i < cycle.size(); ++i) {
        int from = cycle[i];
        int after = cycle[(i + 1) % cycle.size()];
        if ( (other_edges.find({from, after}) == other_edges.end() && other_edges.find({after, from}) == other_edges.end())) {
            current_path.push_back(from);
            visited_map[from] = {-1, 1}; // right element of path - set as free TODO: still probably need info about with path does this vertex belong to
            paths.push_back(current_path);
            current_path.clear();
        }
        else {
            current_path.push_back(from);
            visited_map[from] = {paths.size(), 0}; // regular ve
        }
    }
    // One more check - if last element does have an edge with the first element - add current path to first path
    if (other_edges.find({cycle.back(), cycle.front()}) != other_edges.end() || other_edges.find({cycle.front(), cycle.back()}) != other_edges.end()) {
        paths[0].insert(paths[0].end(), current_path.begin(), current_path.end());
    }
    else {
        paths.push_back(current_path);
    }
    return paths;
}