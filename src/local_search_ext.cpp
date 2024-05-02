#include "../lib/tsp.h"


/*

Multiple start local search 

Local search from various random or randomized starting points. You can create random solutions, or use the randomized heuristics from previous classes.

Pseudo code:

Repeat
    Generate a randomized starting solution x
    Local search (x)
Until stop conditions are met
Return the best solution found
*/

auto TSP::multiple_local_search() -> std::tuple<std::vector<int>, std::vector<int>>
{
    std::vector<int> best_cycle1, best_cycle2;
    int best_objective_value = std::numeric_limits<int>::max();
    int objective_value;
    for (int i = 0; i < params.num_starts; ++i) {
        std::tie(cycle1, cycle2) = local_search();
        objective_value = calc_cycles_len();
        if (objective_value < best_objective_value)
         {
            best_objective_value = objective_value;
            best_cycle1 = cycle1;
            best_cycle2 = cycle2;
        }
    }
    return {best_cycle1, best_cycle2};
}

