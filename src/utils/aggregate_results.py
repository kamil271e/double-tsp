import sys
import itertools
import numpy as np
from tabulate import tabulate

INSTANCES = ["kroA100", "kroB100"]
GREEDY_ALGOS = ["nearest", "expansion", "regret"]
LOCAL_ALGOS = list(itertools.product(["random", "regret"], ["inner", "inter"], ["0", "1"]))
CYCLES_DIR = "../cycles"


def get_greedy_result_table():
    results = []
    for algo in GREEDY_ALGOS:
        algo_results = []
        for instance in INSTANCES:
            data = np.loadtxt(f'{CYCLES_DIR}/L_{algo}_{instance}.txt')
            algo_results.append((int(np.min(data)), int(np.max(data)), int(np.mean(data))))
        results.append(algo_results)

    data = []
    for i, algo in enumerate(GREEDY_ALGOS):
        for j, instance in enumerate(INSTANCES):
            min_val, max_val, mean_val = results[i][j]
            data.append([algo, instance, min_val, max_val, mean_val])

    headers = ["Algo", "Instance", "Min", "Max", "Mean"]
    print(tabulate(data, headers=headers, tablefmt="pretty"))


def get_local_result_table():
    results = []
    for algo in LOCAL_ALGOS:
        algo_results = []
        for instance in INSTANCES:
            data = np.loadtxt(f'{CYCLES_DIR}/L_local_{algo[0]}_{algo[1]}_{algo[2]}_{instance}.txt')
            algo_results.append((int(np.min(data)), int(np.max(data)), int(np.mean(data))))
        results.append(algo_results)
    data = []

    for i, algo in enumerate(LOCAL_ALGOS):
        for j, instance in enumerate(INSTANCES):
            min_val, max_val, mean_val = results[i][j]
            data.append([algo, instance, min_val, max_val, mean_val])

    headers = ["Algo", "Instance", "Min", "Max", "Mean"]
    print(tabulate(data, headers=headers, tablefmt="pretty"))


def get_local_time_table():
    results = []
    for algo in LOCAL_ALGOS:
        algo_results = []
        for instance in INSTANCES:
            data = np.loadtxt(f'{CYCLES_DIR}/T_local_{algo[0]}_{algo[1]}_{algo[2]}_{instance}.txt')
            algo_results.append((np.min(data), np.max(data), np.mean(data)))
        results.append(algo_results)
    data = []

    for i, algo in enumerate(LOCAL_ALGOS):
        for j, instance in enumerate(INSTANCES):
            min_val, max_val, mean_val = results[i][j]
            data.append([algo, instance, min_val, max_val, mean_val])

    headers = ["Algo", "Instance", "Min", "Max", "Mean"]
    print(tabulate(data, headers=headers, tablefmt="pretty"))


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python aggregate_results.py <greedy/local>")
    else:
        if sys.argv[1] == "greedy":
            get_greedy_result_table()
        elif sys.argv[1] == "local":
            get_local_result_table()
            get_local_time_table()
        else:
            print("Invalid argument. Choose between 'greedy' and 'local'")
