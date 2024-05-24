#include <iostream>
#include <unordered_set>
#include <vector>

#include <unordered_set>
#include <vector>
#include <tuple>
#include <algorithm> // for std::swap
#include <unordered_map>


// Struktura pair_hash dla unordered_set
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};


// Funkcja usuwająca krawędzie i zwracająca ścieżki
auto remove_edges(std::vector<int>& cycle, const std::unordered_set<std::pair<int, int>, pair_hash>& other_edges) -> std::vector<std::vector<int>> {
    std::vector<std::vector<int>> paths;
    std::vector<int> current_path;

    for (size_t i = 0; i < cycle.size(); ++i) {

        int before = cycle[(i - 1 + cycle.size()) % cycle.size()];
        int from = cycle[i];
        int after = cycle[(i + 1) % cycle.size()];
    

        std::cout << "Before: " << before << " From: " << from << " After: " << after << std::endl;

        // Jeżeli wierzchołek nie ma połączenia z innym wierzchołkiem, to tworzymy osobny path
        if (other_edges.find({from, after}) == other_edges.end() && other_edges.find({before, from}) == other_edges.end()) {
            current_path.push_back(from);
            paths.push_back(current_path);
            current_path.clear();
        } 

        // Jeżeli wierzchołek ma połączenie z innym wierzchołkiem, to dodajemy go do ścieżki
        else {

            
            if (other_edges.find({from, after}) != other_edges.end())
            {
                current_path.push_back(from);
            }
            else
            { 
                current_path.push_back(from);
                paths.push_back(current_path);
                current_path.clear();
            }
        }


        // Jezeli to ostatni wierzchołek, to sprawdzamy czy ma połączenie z pierwszym wierzchołkiem
        if (i == cycle.size() - 1) {
            if (other_edges.find({from, after}) != other_edges.end()) {
                // Dodajemy pierwszy wierzchołek do ścieżki
                current_path.push_back(after);
            }

        }

    }

    // Dodajemy ostatni wierzchołek do ścieżki
    if (!current_path.empty()) {
        paths.push_back(current_path);
    }

    return paths;
}



int main() {
    std::vector<int> cycle = {1, 2, 3, 4, 5};
    std::unordered_set<std::pair<int, int>, pair_hash> other_edges =  {
        {2,3},
        {3,4},
        {5,1},
    };
    std::vector<std::vector<int>> paths = remove_edges(cycle, other_edges);

    // Wydrukuj ścieżki
    for (const auto& path : paths) {
        for (int vertex : path) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
