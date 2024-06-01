#include "../lib/tsp.h"

auto TSP::hybrid_evolution_algo()
	-> std::tuple<std::vector<int>, std::vector<int>> {
	// Initialization
	// Generate an initial population of 20 solutions using local search
	// methods.
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
    int number_of_iteration = 0;
    while (std::chrono::steady_clock::now() < avg_time)
    {
        number_of_iteration++;
        auto [parent1, parent2] = select_two_parents(population);
        //Print parents  

        auto& [parent1_cycle1, parent1_cycle2] = population[parent1];
        auto& [parent2_cycle1, parent2_cycle2] = population[parent2];

        auto edges1 = findEdges(parent2_cycle1);
        auto edges2 = findEdges(parent2_cycle2);
        std::unordered_set<std::pair<int, int>, pair_hash> edges;
        edges.insert(edges1.begin(), edges1.end());
        edges.insert(edges2.begin(), edges2.end());

        std::map<int, std::pair<int, int>> visited_map1;
        std::map<int, std::pair<int, int>> visited_map2;
        auto c1 = remove_edges(parent1_cycle1, edges); // czy przy tej logice 2 roznie visited_map potrzebne
        auto c2 = remove_edges(parent1_cycle2, edges);

        auto cur_solution = find_greedy_cycles_regret_from_incomplete(c1, c2); 

        // Optional Local Search
         if (params.using_local_search == 1) {
            cur_solution = local_search(std::get<0>(cur_solution), std::get<1>(cur_solution));
         }

        //Find the worst solution in the population
        auto [worst, worst_value] = find_best_worst_solution(population, false);
   
       // if cur_solution is better than the worst solution in the population 
       //Add cur_solution to the population and remove the worst solution
        double improvement_percentage = 5.0; 
        double threshold = (1.0 - improvement_percentage / 100.0) * worst_value;

        if (calculate_objective(std::get<0>(cur_solution), std::get<1>(cur_solution)) < threshold) {
            population.push_back(cur_solution);
            population.erase(population.begin() + worst);
        
        }
 
    }
    //Find the best solution in the population
    auto [best, best_value] = find_best_worst_solution(population, true);
    save_data("I", number_of_iteration, params, "hea");


    return population[best];
}

//Create function for finding the best and worst solution in the population
auto TSP::find_best_worst_solution(std::vector<std::tuple<std::vector<int>, std::vector<int>>> population, bool find_best) -> std::pair<int, int> {
    int best = 0;
    int worst = 0;
    int best_value = calculate_objective(std::get<0>(population[0]), std::get<1>(population[0]));
    int worst_value = calculate_objective(std::get<0>(population[0]), std::get<1>(population[0]));
    for (int i = 1; i < population.size(); i++) {
        int value = calculate_objective(std::get<0>(population[i]), std::get<1>(population[i]));

        if (find_best && value < best_value) {
            best = i;
            best_value = value;
        }
        if (!find_best && value > worst_value) {
            worst = i;
            worst_value = value;
        }
    }
    return find_best ? std::make_pair(best, best_value) : std::make_pair(worst, worst_value);
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


//1. Find edges that do not occur in the second parent
//2. Remove them from the cycle, also remove the vertices that have no more edges from the cycle
//3. Connect the remaining vertices, which occur in both parents, to each other
auto TSP::remove_edges(std::vector<int>&  cycle, const std::unordered_set<std::pair<int, int>, pair_hash>& other_edges) -> std::vector<int>
{
    std::vector<int> new_cycle;
    for (size_t i = 0; i < cycle.size(); ++i) {
        int from = cycle[i];
        int to = cycle[(i + 1) % cycle.size()];
        if (other_edges.find({from, to}) == other_edges.end() && other_edges.find({to, from}) == other_edges.end()) {
            continue;
        }

        // Add from and to varibles to the new cycle, but only if they are not already in the new cycle
        if (std::find(new_cycle.begin(), new_cycle.end(), from) == new_cycle.end()) {
            new_cycle.push_back(from);
        }
        if (std::find(new_cycle.begin(), new_cycle.end(), to) == new_cycle.end()) {
            new_cycle.push_back(to);
        }

    }

    return new_cycle;
}
