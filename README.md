# double-tsp
Assorted heuristics for solving a modified Traveling Salesperson Problem (TSP) combinatorial challenge.

- ![Greedy](greedy.md)
- ![Local search](local_search.md)
- Local search optimizations
- Local search extensions


### Formatting
To format code install clang-format and run script:
```
find . -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format -style="{UseTab: Always, IndentWidth: 4, TabWidth: 4}" -i {} \;
```