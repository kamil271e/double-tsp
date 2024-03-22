import numpy as np
from tabulate import tabulate

INSTANCES = ["kroA100", "kroB100"]
ALGOS = ["nearest", "expansion", "regret"]
CYCLES_DIR = "../cycles"


def get_result_table():
    results = []
    for algo in ALGOS:
        algo_results = []
        for instance in INSTANCES:
            data = np.loadtxt(f'{CYCLES_DIR}/{algo}_{instance}.txt')
            algo_results.append((int(np.min(data)), int(np.max(data)), int(np.mean(data))))
        results.append(algo_results)

    data = []
    for i, algo in enumerate(ALGOS):
        for j, instance in enumerate(INSTANCES):
            min_val, max_val, mean_val = results[i][j]
            data.append([algo, instance, min_val, max_val, mean_val])

    headers = ["Algo", "Instance", "Min", "Max", "Mean"]
    print(tabulate(data, headers=headers, tablefmt="pretty"))


if __name__ == "__main__":
    get_result_table()


