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

	//auto avg_time = calculateAverageMSLStime();

	// TODO: put it in separate function:

    // while (time < avg_time): ...

        // auto [parent1, parent2] = select_two_parents(population);
        // //Print parents
 
        

        // auto& [parent1_cycle1, parent1_cycle2] = population[parent1];
        // auto& [parent2_cycle1, parent2_cycle2] = population[parent2];

        std::vector<int> parent1_cycle1 = {
        98, 114, 88, 148, 28, 39, 38, 71, 130, 72, 83, 62, 185, 168, 50, 139, 86, 196, 56, 152,
        178, 5, 105, 43, 137, 133, 176, 113, 195, 182, 94, 95, 91, 150, 173, 23, 144, 70, 76, 102,
        164, 140, 21, 154, 89, 41, 59, 3, 73, 189, 69, 142, 131, 180, 156, 100, 33, 45, 197, 81,
        97, 104, 165, 166, 96, 126, 87, 52, 11, 84, 170, 48, 119, 92, 175, 10, 19, 99, 14, 36,
        57, 74, 108, 192, 147, 40, 177, 155, 183, 22, 134, 8, 17, 25, 90, 34, 58, 141, 171, 200
        };

        std::vector<int> parent1_cycle2 = {
        115, 117, 111, 12, 46, 172, 121, 174, 159, 24, 32, 136, 61, 138, 26, 199, 7, 82, 78, 9,
        124, 118, 16, 63, 51, 194, 122, 116, 188, 44, 153, 66, 179, 37, 184, 29, 110, 18, 49, 190,
        149, 106, 93, 163, 4, 101, 60, 193, 128, 167, 30, 68, 169, 35, 2, 181, 125, 161, 77, 158,
        80, 65, 151, 187, 6, 54, 129, 146, 103, 143, 75, 109, 107, 157, 31, 67, 47, 120, 112, 127,
        186, 135, 42, 55, 20, 64, 162, 160, 79, 13, 15, 123, 198, 27, 191, 145, 85, 132, 1, 53
         };

        std::vector<int>  parent2_cycle1 =  {
        167, 41, 59, 3, 73, 89, 30, 68, 169, 35, 2, 181, 125, 168, 173, 23, 144, 70, 76, 94,
        182, 195, 113, 176, 133, 137, 43, 105, 5, 86, 139, 50, 185, 62, 83, 72, 130, 71, 196, 178,
        152, 56, 38, 39, 28, 148, 88, 200, 171, 141, 58, 98, 114, 103, 34, 90, 143, 25, 17, 146,
        129, 134, 22, 8, 155, 183, 75, 107, 109, 6, 54, 187, 151, 161, 80, 31, 47, 120, 112, 127,
        186, 135, 42, 55, 15, 123, 198, 27, 191, 145, 85, 132, 1, 53, 115, 117, 111, 40, 12, 147
        };

        std::vector<int>  parent2_cycle2 = {
        7, 199, 26, 61, 136, 32, 24, 159, 174, 121, 172, 46, 65, 177, 13, 79, 160, 20, 64, 162,
        67, 157, 95, 91, 150, 102, 154, 21, 140, 164, 77, 158, 193, 128, 60, 101, 4, 163, 93, 106,
        149, 190, 49, 18, 110, 29, 184, 37, 179, 153, 66, 119, 19, 99, 92, 10, 175, 14, 192, 108,
        142, 69, 189, 131, 180, 74, 57, 36, 100, 156, 33, 45, 197, 81, 97, 104, 165, 166, 96, 126,
        87, 52, 11, 84, 48, 170, 122, 116, 188, 44, 63, 194, 51, 16, 118, 124, 138, 9, 78, 82
        };



        // // Print parents
        // std::cout << "Parent 1: ";
        // for (int vertex : parent1_cycle1) {
        //     std::cout << vertex + 1 << " ";
        // }
        // std::cout << std::endl;
        // for (int vertex : parent1_cycle2) {
        //     std::cout << vertex + 1 << " ";
        // }
        // std::cout << std::endl;

        // std::cout << "Parent 2: ";
        // for (int vertex : parent2_cycle1) {
        //     std::cout << vertex + 1 << " ";
        // }
        // std::cout << std::endl;
        // for (int vertex : parent2_cycle2) {
        //     std::cout << vertex + 1 << " ";
        // }
        // std::cout << std::endl;

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

auto TSP::remove_edges(std::vector<int>& cycle, const std::unordered_set<std::pair<int, int>, pair_hash>& other_edges, std::map<int, std::pair<int, int>>& visited_map) -> std::vector<std::vector<int>> {
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