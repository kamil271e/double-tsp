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

	// Remove edges not present in both parents
    std::cout << "child1: " << child1.size() << std::endl;
	remove_edges(child1, edges2);
    std::cout << "child1: " << child1.size() << std::endl;

	// Remove isolated vertices
	std::unordered_set<int> vertices1(child1.begin(), child1.end());
	std::unordered_set<int> vertices2(child2.begin(), child2.end());

    std::cout << "vertices1: " << child1.size() << std::endl;
	remove_isolated_vertices(child1, vertices2);
    std::cout << "vertices1: " << child1.size() << std::endl;

	// Repair the solutions using the regret heuristic method
	auto repairedCycles =
		find_greedy_cycles_regret_from_incomplete(child1, other_cycle);
	child1 = std::get<0>(repairedCycles);

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

// Function to remove edges not present in both cycles
void TSP::remove_edges(
	std::vector<int> &cycle,
	const std::unordered_set<std::pair<int, int>, pair_hash> &other_edges) {
	for (auto it = cycle.begin(); it != cycle.end();) {
		int from = *it;
		int to =
			*(std::next(it) != cycle.end() ? std::next(it) : cycle.begin());
		if (from > to)
			std::swap(from, to);
		if (other_edges.find({from, to}) == other_edges.end()) {
			it = cycle.erase(it);
			if (it == cycle.end())
				it = cycle.begin();
		} else {
			++it;
		}
	}
}

// Function to remove isolated vertices from the cycle
void TSP::remove_isolated_vertices(std::vector<int> &cycle,
								   const std::unordered_set<int> &vertices) {
	for (auto it = cycle.begin(); it != cycle.end();) {
		std::cout << *it << std::endl;
        if (vertices.find(*it) == vertices.end()) {
			it = cycle.erase(it);
		} else {
			++it;
		}
	}
}

