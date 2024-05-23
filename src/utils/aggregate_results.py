import sys
import os
import re
import itertools
import numpy as np
from tabulate import tabulate

INSTANCES = ["kroA100", "kroB100"]
INSTANCES_BIG = ["kroA200", "kroB200"]
GREEDY_ALGOS = ["nearest", "expansion", "regret"]
LOCAL_ALGOS = list(itertools.product(["random", "regret"], ["edge", "vertex"], ["greedy", "steepest"]))
LOCAL_EXT_ALGOS = ["msls", "ils1", "ils2"]
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


def get_local_ext_result_table():
    results = []
    for algo in LOCAL_EXT_ALGOS:
        algo_results = []
        for instance in INSTANCES_BIG:
            # Construct the expected file path based on the given properties
            file_pattern = f'L_{algo}_(regret|random)_(vertex|edge)_(steepest|greedy)_{instance}.txt'
            file_regex = re.compile(file_pattern)
            matching_files = [file for file in os.listdir(CYCLES_DIR) if file_regex.match(file)]

            if matching_files:
                # Use the first matching file found
                data = np.loadtxt(os.path.join(CYCLES_DIR, matching_files[0]))
                algo_results.append((int(np.min(data)), int(np.max(data)), int(np.mean(data))))
            else:
                print(f"No file found for properties: {algo}, {instance}")

        results.append(algo_results)
    data = []
    for i, algo in enumerate(LOCAL_EXT_ALGOS):
        for j, instance in enumerate(INSTANCES_BIG):
            min_val, max_val, mean_val = results[i][j]
            data.append([algo, instance, min_val, max_val, mean_val])

    headers = ["Algo", "Instance", "Min", "Max", "Mean"]
    print(tabulate(data, headers=headers, tablefmt="pretty"))


# Random Walk Experiments
################################################################################

def get_random_walk_result_table():
    results = []
    for algo in LOCAL_ALGOS:
        algo_results = []
        for instance in INSTANCES:
            data = np.loadtxt(f'{CYCLES_DIR}/L_random_walk_{algo[0]}_{algo[1]}_greedy_{instance}.txt')
            algo_results.append((int(np.min(data)), int(np.max(data)), int(np.mean(data))))
        results.append(algo_results)
    data = []

    for i, algo in enumerate(LOCAL_ALGOS):
        for j, instance in enumerate(INSTANCES):
            min_val, max_val, mean_val = results[i][j]
            data.append([algo, instance, min_val, max_val, mean_val])

    headers = ["Algo", "Instance", "Min", "Max", "Mean"]
    print(tabulate(data, headers=headers, tablefmt="pretty"))


def get_random_walk_table():
    results = []
    for algo in LOCAL_ALGOS:
        algo_results = []
        for instance in INSTANCES:
            data = np.loadtxt(f'{CYCLES_DIR}/T_random_walk_{algo[0]}_{algo[1]}_greedy_{instance}.txt')
            algo_results.append((np.min(data), np.max(data), np.mean(data)))
        results.append(algo_results)
    data = []

    for i, algo in enumerate(LOCAL_ALGOS):
        for j, instance in enumerate(INSTANCES):
            min_val, max_val, mean_val = results[i][j]
            data.append([algo, instance, min_val, max_val, mean_val])

    headers = ["Algo", "Instance", "Min", "Max", "Mean"]
    print(tabulate(data, headers=headers, tablefmt="pretty"))

################################################################################


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python aggregate_results.py <greedy/local>")
    else:
        if sys.argv[1] == "greedy":
            get_greedy_result_table()
        elif sys.argv[1] == "local":
            get_local_result_table()
            get_local_time_table()
        elif sys.argv[1] == "random_walk":
            get_random_walk_result_table()
            get_random_walk_table()
        elif sys.argv[1] == "local_ext":
            get_local_ext_result_table()
        else:
            print("Invalid argument. Choose between 'greedy' and 'local'")
