import numpy as np
import matplotlib.pyplot as plt
import networkx as nx
import sys
import os
from get_coords_per_instance import get_coords

CYCLES_DIR = "../cycles"
PLOTS_DIR = '../plots'
G1_COLOR = 'skyblue'
G2_COLOR = 'salmon'
NODE_SIZE = 600
EDGE_WIDTH = 2


def read_lines(file_path, i):
    lines = []
    file_path = f'{CYCLES_DIR}/{file_path}'
    with open(file_path, 'r') as file:
        for _ in range(i + 2):
            line = file.readline().strip()
            if not line:
                break
            lines.append(line)
    if len(lines) < 2:
        return None
    return '\n'.join(lines[-2:])


def visualize_tsp_cycles(path, cycles, plot_name, title='TSP Cycles'):
    numbers_lists = cycles.split("\n")
    cycle1_nodes = list(map(int, numbers_lists[0].split()))
    cycle2_nodes = list(map(int, numbers_lists[1].split()))
    node_positions = get_coords(path)

    G1 = nx.Graph()
    G1.add_nodes_from(cycle1_nodes)
    G1.add_edges_from(zip(cycle1_nodes, cycle1_nodes[1:] + [cycle1_nodes[0]]))
    nx.set_node_attributes(G1, node_positions, "pos")

    G2 = nx.Graph()
    G2.add_nodes_from(cycle2_nodes)
    G2.add_edges_from(zip(cycle2_nodes, cycle2_nodes[1:] + [cycle2_nodes[0]]))
    nx.set_node_attributes(G2, node_positions, "pos")

    plt.figure(figsize=(20, 20))

    pos1 = nx.get_node_attributes(G1, "pos")
    nx.draw(
        G1,
        pos1,
        with_labels=True,
        node_color=G1_COLOR,
        edge_color=G1_COLOR,
        node_size=NODE_SIZE,
        width=EDGE_WIDTH,
        label="Cycle 1",
    )

    pos2 = nx.get_node_attributes(G2, "pos")
    nx.draw(
        G2,
        pos2,
        with_labels=True,
        node_color=G2_COLOR,
        edge_color=G2_COLOR,
        node_size=NODE_SIZE,
        width=EDGE_WIDTH,
        label="Cycle 2",
    )

    plt.suptitle(title, fontsize=20)
    # plt.legend()
    plt.savefig(os.path.join(PLOTS_DIR,f"{plot_name}.png"))


def choose_best_greedy():
    for length_file in os.listdir(CYCLES_DIR):
        if length_file.endswith(".txt") and length_file.startswith('L'):
            parts = length_file.split("_")
            algo, instance = parts[1],  parts[2].split(".")[0]
            path = os.path.join(CYCLES_DIR, length_file)
            data = np.loadtxt(path)
            idx = np.argmin(data)

            cycles_file = length_file[2:] # no "L_" prefix

            cycles = read_lines(cycles_file, 2*idx)
            title = f'{cycles_file[:-4]}_{np.min(data)}'
            plot_name = cycles_file[:-4]
            visualize_tsp_cycles(f'{instance}.tsp', cycles, plot_name, title)


def generate_all_greedy():
    for cycles_file in os.listdir(CYCLES_DIR):
        parts = cycles_file.split("_")
        algo, instance = parts[0],  parts[1].split(".")[0]
        path = os.path.join(CYCLES_DIR, cycles_file)
        data = np.loadtxt(path)
        idx = np.argmin(data)

        cycles = read_lines(cycles_file, 2*idx)
        title = rf'{cycles_file[:-4]}: $\bf{{{int(np.min(data))}}}$'
        plot_name = cycles_file[:-4]
        visualize_tsp_cycles(f'{instance}.tsp', cycles, plot_name, title)


def choose_best_local():
    for length_file in os.listdir(CYCLES_DIR):
        if length_file.endswith(".txt") and length_file.startswith('L'):
            parts = length_file.split("_")
            algo, start_type, movement_type, greedy_or_steepest, instance = parts[1], parts[2], parts[3], parts[4], parts[5].split(".")[0]
            path = os.path.join(CYCLES_DIR, length_file)
            data = np.loadtxt(path)
            idx = np.argmin(data)

            cycles_file = length_file[2:] # no "L_" prefix

            cycles = read_lines(cycles_file, 2*idx)
            title = rf'{cycles_file[:-4]}: $\bf{{{int(np.min(data))}}}$'
            plot_name = cycles_file[:-4]
            visualize_tsp_cycles(f'{instance}.tsp', cycles, plot_name, title)


def choose_best_local_ext():
    for length_file in os.listdir(CYCLES_DIR):
        if length_file.endswith(".txt") and length_file.startswith('L'):
            parts = length_file.split("_")
            algo, start_type, movement_type, greedy_or_steepest, instance = parts[1], parts[2], parts[3], parts[4], parts[5].split(".")[0]
            path = os.path.join(CYCLES_DIR, length_file)

            data = np.loadtxt(path)
            idx = np.argmin(data)

            cycles_file = length_file[2:] # no "L_" prefix
            cycles = read_lines(cycles_file, 2*idx)
            title = rf'{cycles_file[:-4]}: $\bf{{{int(np.min(data))}}}$'
            plot_name = cycles_file[:-4]
            visualize_tsp_cycles(f'{instance}.tsp', cycles, plot_name, title)

def choose_best_hea():
    for length_file in os.listdir(CYCLES_DIR):
        if length_file.endswith(".txt") and length_file.startswith('L'):
            parts = length_file.split("_")
            print(parts)
            if len(parts) == 6: 
                algo, start_type, movement_type, greedy_or_steepest, instance = parts[1], parts[2], parts[3], parts[4],  parts[5].split(".")[0]
            else:
                algo, start_type, movement_type, greedy_or_steepest, local, instance = parts[1], parts[2], parts[3], parts[4], parts[5], parts[6].split(".")[0]
            path = os.path.join(CYCLES_DIR, length_file)

            data = np.loadtxt(path)
            idx = np.argmin(data)

            cycles_file = length_file[2:] # no "L_" prefix
            cycles = read_lines(cycles_file, 2*idx)
            title = rf'{cycles_file[:-4]}: $\bf{{{int(np.min(data))}}}$'
            plot_name = cycles_file[:-4]
            visualize_tsp_cycles(f'{instance}.tsp', cycles, plot_name, title)




# Random Walk Experiments
################################################################################

def choose_best_random_walk():
    for length_file in os.listdir(CYCLES_DIR):
        if length_file.endswith(".txt") and length_file.startswith('L'):
            
            algo = "random_walk"
            parts = length_file.split("_")
            start_type, movement_type, instance =  parts[3], parts[4],  parts[6].split(".")[0]
            print(start_type, movement_type, instance)
            path = os.path.join(CYCLES_DIR, length_file)
            data = np.loadtxt(path)
            idx = np.argmin(data)

            cycles_file = algo + "_" + start_type + "_" +  movement_type + "_" + instance # no "L_" prefix

            cycles = read_lines(length_file[2:], 2*idx)
            title = rf'{cycles_file}: $\bf{{{int(np.min(data))}}}$'
            plot_name = cycles_file
            visualize_tsp_cycles(f'{instance}.tsp', cycles, plot_name, title)

################################################################################


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python generate_visualizations.py <greedy/local>")
    else:
        if sys.argv[1] == "greedy":
            choose_best_greedy()
        elif sys.argv[1] == "local":
            choose_best_local()
        elif sys.argv[1] == "random_walk":
            choose_best_random_walk()
        elif sys.argv[1] == "local_ext":
            choose_best_local_ext()
        elif sys.argv[1] == "hea":
            choose_best_hea()
        else:
            print("Invalid argument. Choose between 'greedy' and 'local'")