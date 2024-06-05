# double-tsp
Assorted heuristics for solving a modified Traveling Salesperson Problem (TSP) combinatorial challenge.

- ![Greedy](reports/greedy.md)
- ![Local search](reports/local_search.md)
- ![Local search optimizations](reports/local_optim.md)
- ![Local search extensions](reports/local_ext.md)
- ![Hybrid evolutionary algorithm](reports/hea.md)


### Formatting
To format code install clang-format and run script:
```
find . -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format -style="{UseTab: Always, IndentWidth: 4, TabWidth: 4}" -i {} \;
```
