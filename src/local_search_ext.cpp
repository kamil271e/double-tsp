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

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < params.num_starts; ++i) {
        std::tie(cycle1, cycle2) = local_search();
        objective_value = calculate_objective(cycle1,cycle2);
        if (objective_value < best_objective_value)
         {
            best_objective_value = objective_value;
            best_cycle1 = cycle1;
            best_cycle2 = cycle2;
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    save_time(duration, params, "multiple_search");

    //TODO: Find a reason for the duplicate vertices in the cycle
    // Temporary solution to the problem of duplicate vertices in the cycle
    best_cycle1 = delete_duplicates(best_cycle1);
    best_cycle2 = delete_duplicates(best_cycle2);

    return {best_cycle1, best_cycle2};
}


// Function to calculate objective value of the cycle
int TSP::calculate_objective(const std::vector<int>& cycle1, const std::vector<int>& cycle2)
{
    int objective_value = 0;
    for (size_t i = 0; i < cycle1.size(); ++i) {
        objective_value += dist_matrix.dist_matrix[cycle1[i]][cycle1[(i + 1) % cycle1.size()]];
        objective_value += dist_matrix.dist_matrix[cycle2[i]][cycle2[(i + 1) % cycle2.size()]];
    }
   
    return objective_value;

}


// Temporary solution to the problem of duplicate vertices in the cycle
// Function to delete duplicate vertices in the cycle
auto TSP::delete_duplicates(const std::vector<int>& cycle) -> std::vector<int>
{
    std::vector<int> new_cycle;
    for (int i = 0; i < cycle.size(); ++i) {
        if (std::find(new_cycle.begin(), new_cycle.end(), cycle[i]) == new_cycle.end()) {
            new_cycle.push_back(cycle[i]);
        }
    }
    return new_cycle;
}