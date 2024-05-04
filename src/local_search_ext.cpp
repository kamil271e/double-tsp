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
        std::cout << "Regret" << std::endl;
        std::tie(cycle_x1, cycle_x2) = find_greedy_cycles_regret();
    }


    //Find avarage value of MSLS time
    long long avg_time = calculateAverage("/home/wladyka/Study/IMO/double-tsp/cycles/T_multiple_search_regret_vertex_steepest_kroA100.txt");
    std::chrono::milliseconds duration(avg_time);
    auto avg_time_in_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    auto target_time = std::chrono::steady_clock::now() + avg_time_in_milliseconds;

    //x := Local search (x)
    std::tie(cycle_x1, cycle_x2) = local_search(cycle_x1, cycle_x2);

   

    //Create loop, where avg_time is the stop condition
    while(std::chrono::steady_clock::now() < target_time)
    {
        // y := x
        cycle_y1 = cycle_x1;
        cycle_y2 = cycle_x2;

        std::cout << "y := x" << std::endl;
        std::cout << "Cycle 1: ";
        for (int i = 0; i < cycle_y1.size(); ++i) {
            std::cout << cycle_y1[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Cycle 2: ";
        for (int i = 0; i < cycle_y2.size(); ++i) {
            std::cout << cycle_y2[i] << " ";
        }
        std::cout << std::endl;

       

        // Perturbation (y)
        std::tie(cycle_y1, cycle_y2) = perturbation_one(cycle_y1, cycle_y2);

        

        std::cout << "Perturbation (y)" << std::endl;
        std::cout << "Cycle 1: ";
        for (int i = 0; i < cycle_y1.size(); ++i) {
            std::cout << cycle_y1[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "Cycle 2: ";
        for (int i = 0; i < cycle_y2.size(); ++i) {
            std::cout << cycle_y2[i] << " ";
        }
        std::cout << std::endl;
        break;

        // y := Local search (y)
        std::tie(cycle_y1, cycle_y2) = local_search(cycle_y1, cycle_y2);

        

        // If f(y) > f(x) then x := y
        if (calculate_objective(cycle_y1, cycle_y2) > calculate_objective(cycle_x1, cycle_x2))
        {
            cycle_x1 = cycle_y1;
            cycle_x2 = cycle_y2;
        }
    }

    return {cycle_x1, cycle_x2};
}

auto TSP::perturbation_one(std::vector<int> &cycle1, std::vector<int> &cycle2) -> std::tuple<std::vector<int>, std::vector<int>>
{

    
    // Randomly select the number of vertices to be replaced
    int num_vertices = rand() % (cycle1.size() / 8) + 1;

    // Randomly select the vertices to be replaced
    std::vector<int> vertices;
    for (int i = 0; i < num_vertices; ++i) {
        int vertex = rand() % dist_matrix.x_coord.size();
        if (std::find(vertices.begin(), vertices.end(), vertex) == vertices.end()) {
            vertices.push_back(vertex);
        }
    }

    // Initialize the movement vector with the selected vertices
    std::vector<int> movement;
    int cycle_num = -1;
    int movement_type;
    int vertex;

    // Replace the selected vertices with random vertices with the random movement type
    for (int i = 0; i < vertices.size(); ++i) {
        
        // Randomly select the vertex to be replaced
        vertex = rand() % dist_matrix.x_coord.size();

        //Create random movement inter or inner (0 or 1)
        movement_type = rand() % 2;
    
        //Check if the vertices are in the same сycle
        if (std::find(cycle1.begin(), cycle1.end(), vertex) != cycle1.end() || std::find(cycle2.begin(), cycle2.end(), vertex) != cycle2.end())
        {
            //Find the cycle number
            cycle_num = (std::find(cycle2.begin(), cycle2.end(), vertex) != cycle2.end()) ? 1 : 0;
            movement = {i, vertex, 0, movement_type, cycle_num};
        }
        else
        {
            movement = {i, vertex, 1, movement_type};
        }
        
        apply_movement(movement, cycle_num);
    }

    

    return {cycle1, cycle2};

}


double TSP::calculateAverage(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 0.0; // Or handle the error in some other way
    }

    std::string line;
    double sum = 0.0;
    int count = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double value;
        while (iss >> value) {
            sum += value;
            count++;
        }
    }

    file.close();

    if (count == 0) {
        std::cerr << "No values found in the file." << std::endl;
        return 0.0; // Or handle this case in some other way
    }

    return sum / count;
}