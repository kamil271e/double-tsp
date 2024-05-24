#include "../lib/tsp.h"

auto TSP::hybrid_evolution_algo()
	-> std::tuple<std::vector<int>, std::vector<int>> {
	// Initialization
	// Generate an initial population of 20 solutions using local search
	// methods.
	bool using_local_search = true;
	std::vector<std::tuple<std::vector<int>, std::vector<int>>> population;
	for (int i = 0; i < 20; i++) {
		auto [cycle1, cycle2] = local_search();
		population.push_back({cycle1, cycle2});
	}

	// Ensure no duplicate solutions exist in the population.
	std::sort(population.begin(), population.end());
	population.erase(std::unique(population.begin(), population.end()),
					 population.end());

	auto avg_time = calculateAverageMSLStime();

	// Selection
	auto [parent1, parent2] = select_two_parents(population);

	// Recombination
	auto parent1_cycle1 = std::get<0>(population[parent1]);
	auto parent1_cycle2 = std::get<1>(population[parent1]);

	auto parent2_cycle1 = std::get<0>(population[parent2]);
	auto parent2_cycle2 = std::get<1>(population[parent2]);

	// Recombination cycle1
	auto child1 = recombine(parent1_cycle1, parent2_cycle1, parent1_cycle2);

	// Recombination cycle2
	auto child2 = recombine(parent1_cycle2, parent2_cycle2, parent2_cycle1);

	return {child1, child2};
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

//// Recombination cycle1
// auto child1 = recombine(parent1_cycle1, parent2_cycle1, parent1_cycle2);
//
//// Recombination cycle2
// auto child2 = recombine(parent1_cycle2, parent2_cycle2, parent2_cycle1);

// Recombination Operator
auto TSP::recombine(const std::vector<int> &parent1,
					const std::vector<int> &parent2,
					std::vector<int> &other_cycle) -> std::vector<int> {
	// Initialize child cycles
	std::vector<int> child1 = parent1;
	std::vector<int> child2 = parent2;

	// Find edges in both parents
	auto edges1 = findEdges(parent1);
	auto edges2 = findEdges(parent2);

	// Merge the edges of both parents in a single set other_edges
    std::unordered_set<std::pair<int, int>, pair_hash> edges;
    edges.insert(edges1.begin(), edges1.end());
    edges.insert(edges2.begin(), edges2.end());

    // Remove edges from the other cycle
    auto paths = remove_edges(other_cycle, edges);


	return child1;
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

auto TSP::remove_edges(std::vector<int>& cycle, const std::unordered_set<std::pair<int, int>, pair_hash>& other_edges) -> std::vector<std::vector<int>> {
    std::vector<std::vector<int>> paths;
    std::vector<int> current_path;

    for (size_t i = 0; i < cycle.size(); ++i) {
        int from = cycle[i];
        int after = cycle[(i + 1) % cycle.size()];
        if ( (other_edges.find({from, after}) == other_edges.end() && other_edges.find({after, from}) == other_edges.end())) {
            current_path.push_back(from);
            paths.push_back(current_path);
            current_path.clear();
        }
        else {
            current_path.push_back(from);
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