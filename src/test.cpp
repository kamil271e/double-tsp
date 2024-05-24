#include <iostream>
#include <unordered_set>
#include <vector>

#include <unordered_set>
#include <vector>
#include <tuple>
#include <algorithm> // for std::swap
#include <unordered_map>


struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};


auto remove_edges(std::vector<int>& cycle, const std::unordered_set<std::pair<int, int>, pair_hash>& other_edges) -> std::vector<std::vector<int>> {
    std::vector<std::vector<int>> paths;
    std::vector<int> current_path;

    for (size_t i = 0; i < cycle.size(); ++i) {
        int from = cycle[i];
        int after = cycle[(i + 1) % cycle.size()];
        std::cout << "From: " << from << " After: " << after << std::endl;
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



int main() {
    std::vector<int> cycle = {1, 2, 3, 4, 5};
    std::unordered_set<std::pair<int, int>, pair_hash> other_edges =  {
            {1,2},
            //{2,3},
            {3,4},
            {5,1},
    };
    std::vector<std::vector<int>> paths = remove_edges(cycle, other_edges);

    for (const auto& path : paths) {
        for (int vertex : path) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
