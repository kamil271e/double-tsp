#include "../lib/tsp.h"
#include <iostream>
#include <vector>

void generate_cycles(TSP tsp) {
	auto [cycle1, cycle2] = tsp.solve();

	for (int vertex : cycle1) {
		std::cout << vertex + 1 << " ";
	}
	std::cout << std::endl;

	for (int vertex : cycle2) {
		std::cout << vertex + 1 << " ";
	}
	std::cout << std::endl;
}

AlgType choose_algo(std::string algo) {
	if (algo == "nearest") {
		return AlgType::nearest_neighbors;
	} else if (algo == "expansion") {
		return AlgType::greedy_cycle;
	} else if (algo == "regret") {
		return AlgType::regret;
	} else if (algo == "local") {
		return AlgType::local;
	} else if (algo == "random_walk") {
		return AlgType::random_walk;
	} else if (algo == "msls") {
		return AlgType::multiple_local_search;
	} else if (algo == "ils1") {
		return AlgType::ils1;
	} else if (algo == "ils2") {
		return AlgType::ils2;
	} else if (algo == "hea") {
		return AlgType::hea;
	} else {
		std::cerr << "Invalid algorithm type. Please choose from [nearest, "
					 "expansion, regret, local]"
				  << std::endl;
		exit(1);
	}
}

int main(int argc, char *argv[]) {

	// NEAREST | EXPANSION | REGRET |
	if (argc >= 3 && argc < 6) {
		Matrix m;
		m.load_from_path(argv[1]);
		m.generate_dist_matrix();
		AlgType alg_type = choose_algo(std::string(argv[2]));
		TSP tsp(m, alg_type);
		generate_cycles(tsp);
		return 0;

	} else if (argc < 3) {
		std::cerr
			<< "Usage: " << argv[0]
			<< " <filename> <algotype> {nearest, expansion, regret} [start_idx]"
			<< std::endl;
		return 1;
	}

	// LOCAL
	if (argc >= 6) {

		Matrix m;
		m.load_from_path(argv[1]);
		m.generate_dist_matrix();
		LocalSearchParams params;
		params.filename = argv[1];
		params.input_data = std::string(argv[3]);
		params.movements_type = std::string(argv[4]);
		params.steepest = std::stoi(argv[5]);

		AlgType alg_type = choose_algo(std::string(argv[2]));
		if (alg_type == AlgType::hea || alg_type == AlgType::ils2) {
			params.using_local_search = std::stoi(argv[6]);			
			TSP tsp(m, alg_type, params.input_data, params.movements_type,
					params.steepest, params.using_local_search, params.filename);

			generate_cycles(tsp);
			return 0;
		}
		// Print alg_type
		TSP tsp(m, alg_type, params.input_data, params.movements_type,
				params.steepest, params.filename);
		generate_cycles(tsp);
		return 0;
	} else if (argc < 6) {
		std::cerr << "Usage: " << argv[0]
				  << " <instance_path> <algotype> [nearest, expansion, regret, "
					 "local, msls, ils1, ils2, hea ]"
				  << " <input_data> [random, regret] <movements_type> [inner, "
					 "inter] <greedy/steepest> [0, 1] "
				  << std::endl;
		return 1;
	}

	return 0;
}