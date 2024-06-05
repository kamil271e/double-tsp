#include <iostream>
#include <unordered_set>
#include <vector>

#include <unordered_set>
#include <vector>
#include <tuple>
#include <algorithm> // for std::swap
#include <unordered_map>
#include <map>


enum available {
    NOT=-2,
    FREE=-1,
};

enum side {
    LEFT=-1,
    RIGHT=1
};

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};


auto remove_edges(std::vector<int>& cycle, const std::unordered_set<std::pair<int, int>, pair_hash>& other_edges, std::map<int, std::pair<int, int>>& visited_map) -> std::vector<std::vector<int>> {
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
        if (current_path.size() > 0){
            visited_map[current_path.back()] = {paths.size(), side(RIGHT)};
            paths.push_back(current_path);
        }
    }
    return paths;
}



int main() {
    std::map<int, std::pair<int, int>> visited_map;
    std::vector<int> cycle = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::unordered_set<std::pair<int, int>, pair_hash> other_edges =  {
//            {1,2},
            {2,3},
//            {3,4},
            {4,5},
            {5,6},
//            {6,7},
            {7,8},
            {8,9},
//            {9,1},
    };
    std::vector<std::vector<int>> paths = remove_edges(cycle, other_edges, visited_map);

    for (const auto& path : paths) {
        for (int vertex : path) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "MAP" << std::endl;

    for (const auto& [vertex, info] : visited_map) {
        std::cout << vertex << " " << info.first << " " << info.second << std::endl;
    }

    return 0;
}
