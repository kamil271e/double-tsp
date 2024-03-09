import numpy as np

INSTANCES = ["kroA100", "kroB100"]
ALGOS = ["nearest", "expansion"]
CYCLES_DIR = "../cycles"


def get_result_table():
    results = []
    for algo in ALGOS:
        algo_results = []
        for instance in INSTANCES:
            data = np.loadtxt(f'{CYCLES_DIR}/{algo}_{instance}.txt')
            algo_results.append((np.min(data), np.max(data), round(np.mean(data),1)))
        results.append(algo_results)

    print("Algo\t\tInstance\tMin\tMax\tMean")
    for i, algo in enumerate(ALGOS):
        for j, instance in enumerate(INSTANCES):
            min_val, max_val, mean_val = results[i][j]
            print(f"{algo}\t\t{instance}\t{min_val}\t{max_val}\t{mean_val}")


if __name__ == "__main__":
    get_result_table()


