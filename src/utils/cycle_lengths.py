import sys
import os
from get_coords_per_instance import get_coords

RESULTS_DIR = '../cycles'

def get_cycle_lengths(instance_path, cycles):
    numbers_lists = cycles.split("\n")
    cycle1_nodes = list(map(int, numbers_lists[0].split()))
    cycle2_nodes = list(map(int, numbers_lists[1].split()))
    node_positions = get_coords(instance_path)

    len_cycle1 = 0
    for i in range(len(cycle1_nodes)):
        len_cycle1 += get_length(
            node_positions[cycle1_nodes[i]],
            node_positions[cycle1_nodes[(i + 1) % len(cycle1_nodes)]],
        )
    len_cycle2 = 0
    for i in range(len(cycle1_nodes)):
        len_cycle2 += get_length(
            node_positions[cycle2_nodes[i]],
            node_positions[cycle2_nodes[(i + 1) % len(cycle2_nodes)]],
        )

    return len_cycle1, len_cycle2


def get_length(node1, node2):
    x1, y1 = node1
    x2, y2 = node2
    return round(((y1 - y2) ** 2 + (x1 - x2) ** 2) ** (1 / 2))


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python cycle_lengths.py <instance_path> <cycles>")
        if not os.path.exists(RESULTS_DIR):
            os.makedirs(RESULTS_DIR)
    else:
        if not os.path.exists(RESULTS_DIR):
            os.makedirs(RESULTS_DIR)
        instance_path, cycles = sys.argv[1], sys.argv[2]
        c1, c2 = get_cycle_lengths(instance_path, cycles)
        print(c1 + c2)
