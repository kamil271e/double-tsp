import matplotlib.pyplot as plt
import networkx as nx
import sys
import os
from get_cycles import get_cycles

PLOTS_DIR = '../plots'
G1_COLOR = 'skyblue'
G2_COLOR = 'salmon'
NODE_SIZE = 400


def visualize_tsp_cycles(path, cycles, plot_name):
    numbers_lists = cycles.split("\n")
    cycle1_nodes = list(map(int, numbers_lists[0].split()))
    cycle2_nodes = list(map(int, numbers_lists[1].split()))
    node_positions = get_cycles(path)

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
        label="TSP Cycle 1",
    )

    pos2 = nx.get_node_attributes(G2, "pos")
    nx.draw(
        G2,
        pos2,
        with_labels=True,
        node_color=G2_COLOR,
        edge_color=G2_COLOR,
        node_size=NODE_SIZE,
        label="TSP Cycle 2",
    )

    plt.title("TSP Cycles")
    plt.legend()
    plt.savefig(os.path.join(PLOTS_DIR,f"{plot_name}.png"))


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python generate_visualization.py <file_path> <plot_name> <cycles>")
    else:
        if not os.path.exists(PLOTS_DIR):
            os.makedirs(PLOTS_DIR)
        path = sys.argv[1]
        plot_name = sys.argv[2]
        cycles = sys.argv[3]
        visualize_tsp_cycles(path, cycles, plot_name)
