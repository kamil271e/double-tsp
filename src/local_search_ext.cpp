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


/*
Iterative local search with little perturbation. Perturbation1 (ILS1) can involve, for example, replacing several edges and/or vertices with others selected at random. The stop condition for ILSx is to reach a time equal to the average MSLS time for the same instance.

Attention , one run of MSLS includes 100 iterations of LS, and the the final result is the best solution obtained in those 100 runs. The starting solution can be a random solution or one obtained using a randomized heuristic.

Pseudo code:

Generate the initial solution x
x := Local search (x)
Repeat
    y := x
    Perturbation (y)
    y := Local search (y)
    If f(y) > f(x) then
         x := y
To meet the stop conditions
*/

auto TSP::iterative_local_search_one() -> std::tuple<std::vector<int>, std::vector<int>>
{

    std::vector<int> cycle_x1, cycle_x2;
    std::vector<int> cycle_y1, cycle_y2;

    //Generate the initial solution x
     if (params.input_data == "random") {
        std::tie(cycle_x1, cycle_x2) = generate_random_cycles(100);

    }
    else if(params.input_data == "regret") {
        std::tie(cycle_x1, cycle_x2) = find_greedy_cycles_regret();
    }

    //Find avarage value of MSLS time
    long long avg_time = calculateAverage("/home/wladyka/Study/IMO/double-tsp/cycles/T_multiple_search_regret_vertex_steepest_kroA200.txt");
    std::chrono::milliseconds duration(avg_time);
    auto avg_time_in_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    auto target_time = std::chrono::steady_clock::now() + avg_time_in_milliseconds;

    auto start_time = std::chrono::high_resolution_clock::now();

    //x := Local search (x)
    std::tie(cycle_x1, cycle_x2) = local_search(cycle_x1, cycle_x2);
    //Create loop, where avg_time is the stop condition
    while(std::chrono::steady_clock::now() < target_time)
    {
        // y := x
        cycle_y1 = cycle_x1;
        cycle_y2 = cycle_x2;
       
        // Perturbation (y)
        std::tie(cycle_y1, cycle_y2) = perturbation_one(cycle_y1, cycle_y2);

        // y := Local search (y)
        std::tie(cycle_y1, cycle_y2) = local_search(cycle_y1, cycle_y2);

        
        // If f(y) > f(x) then x := y
        if (calculate_objective(cycle_y1, cycle_y2) < calculate_objective(cycle_x1, cycle_x2))
        {
            cycle_x1 = cycle_y1;
            cycle_x2 = cycle_y2;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto operating_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    save_time(operating_time, params, "iswd1");

    return {cycle_x1, cycle_x2};
}

auto TSP::perturbation_one(std::vector<int> &c1, std::vector<int> &c2) -> std::tuple<std::vector<int>, std::vector<int>>
{

    // Randomly select the number of vertices to be replaced
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> num_vertices_dist(1, c1.size() / 8);
    int num_vertices = num_vertices_dist(gen);

    // Randomly select the vertices to be replaced
    std::vector<int> vertices;
    std::uniform_int_distribution<int> vertex_dist(0, dist_matrix.x_coord.size() - 1);
    while (vertices.size() < num_vertices) {
        int vertex = vertex_dist(gen);
        if (std::find(vertices.begin(), vertices.end(), vertex) == vertices.end()) {
            vertices.push_back(vertex);
        }
    }

    // Replace the selected vertices with random vertices with the random movement type
    for (int i = 0; i < vertices.size(); ++i) {
        
        // Randomly select the vertex to be replaced
        int j = vertex_dist(gen);
       
        //Create random movement edge or vertex (0 or 1)
        std::uniform_int_distribution<int> movement_type_dist(0, 1);
        int movement_type = movement_type_dist(gen);
    
        // Find the cycle in which the selected vertex is located
        auto& num_cycle_i = std::find(c1.begin(), c1.end(), vertices[i]) != c1.end() ? c1 : c2;
        auto& num_cycle_j = std::find(c1.begin(), c1.end(), j) != c1.end() ? c1 : c2;

        //Find index of selected vertex
        int idx_i = std::find(num_cycle_i.begin(), num_cycle_i.end(), vertices[i]) - num_cycle_i.begin();
        int idx_j = std::find(num_cycle_j.begin(), num_cycle_j.end(), j) - num_cycle_j.begin();

        if (vertices[i] != j){
            // Check if the selected vertex is in the same num_cycle_j
            if (std::find(num_cycle_j.begin(), num_cycle_j.end(), j) != num_cycle_j.end() && std::find(num_cycle_j.begin(), num_cycle_j.end(), vertices[i]) != num_cycle_j.end()) { 
                //Update the num_cycle_j (inner class)
                if (movement_type == 0){ // edge
                    std::reverse(num_cycle_j.begin() + idx_i, num_cycle_j.begin() + idx_j + 1);
                } else { // vertex
                    std::swap(num_cycle_j[idx_i], num_cycle_j[idx_j]);
                }
            }
            else //Update the cycles(inter class)
            {
                int temp = c1[idx_i];
                c1[idx_i] = c2[idx_j];
                c2[idx_j] = temp;
            }
        }
        
    }

    return {c1, c2};

}

/*
Iterative local search with Large-scale neighborhood search, i.e., a larger Destroy-Repair perturbation.

Perturbation2 (ILS2) should involve removing more edges and vertices (e.g., 30%) (destroy) and fixing the solution using a heuristic method, one of those implemented in the first lesson. The vertices/edges to be removed can be chosen randomly or heuristically, such as those that are close to the second cycle. We also test this version without a local search (only subject the initial solution to a local search, as long as the starting solution was random) The stop condition for ILSx is to reach a time equal to the average MSLS time for the same instance.

Attention , one run of MSLS includes 100 iterations of LS, and the the final result is the best solution obtained in those 100 runs. The starting solution can be a random solution or one obtained using a randomized heuristic.

Pseudo code:

Generate the initial solution x
x := Local search (x) (option)
Repeat
    y := x
    Destroy (y)
    Repair (y)
    y := Local search (y) (option)
    If f(y) > f(x) then
         x := y
To meet the stop conditions
*/


auto TSP::iterative_local_search_two() -> std::tuple<std::vector<int>, std::vector<int>>
{
    std::vector<int> cycle_x1, cycle_x2;
    std::vector<int> cycle_y1, cycle_y2;

    //Generate the initial solution x
     if (params.input_data == "random") {
        std::tie(cycle_x1, cycle_x2) = generate_random_cycles(100);

    }
    else if(params.input_data == "regret") {
        std::tie(cycle_x1, cycle_x2) = find_greedy_cycles_regret();
    }

    //Find avarage value of MSLS time
    long long avg_time = calculateAverage("/home/wladyka/Study/IMO/double-tsp/cycles/T_multiple_search_regret_vertex_steepest_kroA200.txt");
    std::chrono::milliseconds duration(avg_time);
    auto avg_time_in_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    auto target_time = std::chrono::steady_clock::now() + avg_time_in_milliseconds;

    auto start_time = std::chrono::high_resolution_clock::now();

    //?????? x := Local search (x) (option) ????? 
    //I'm not sure about this line, as I don't understand what the option means 
    std::tie(cycle_x1, cycle_x2) = local_search(cycle_x1, cycle_x2);
    //Create loop, where avg_time is the stop condition
    while(std::chrono::steady_clock::now() < target_time)
    {
        // y := x
        cycle_y1 = cycle_x1;
        cycle_y2 = cycle_x2;

        //TODO: Destroy (y) 
        std::tie(cycle_y1, cycle_y2) = destroy_perturbation(cycle_y1, cycle_y2);

        //TODO: Repair (y)
        std::tie(cycle_y1, cycle_y2) = repair_perturbation(cycle_y1, cycle_y2);

        // y := Local search (y)  (option)
        //I'm not sure about this line, as I don't understand what the option means 
        std::tie(cycle_y1, cycle_y2) = local_search(cycle_y1, cycle_y2);

        // If f(y) > f(x) then x := y
        if (calculate_objective(cycle_y1, cycle_y2) < calculate_objective(cycle_x1, cycle_x2))
        {
            cycle_x1 = cycle_y1;
            cycle_x2 = cycle_y2;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto operating_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    save_time(operating_time, params, "ils2");

    return {cycle_x1, cycle_x2};


}

auto TSP::destroy_perturbation(std::vector<int> &c1, std::vector<int> &c2) -> std::tuple<std::vector<int>, std::vector<int>>
{
    //TODO: implement destroy perturbation

    return {c1, c2};
}

auto TSP::repair_perturbation(std::vector<int> &c1, std::vector<int> &c2) -> std::tuple<std::vector<int>, std::vector<int>>
{
    
    //TODO: implement repair perturbation

    return {c1, c2};
}