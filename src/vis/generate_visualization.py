import matplotlib.pyplot as plt
import networkx as nx
import sys


def visualize_tsp_cycles(path, cycles):
    numbers_lists = cycles.split('\n')
    cycle1_nodes = list(map(int, numbers_lists[0].split()))
    cycle2_nodes = list(map(int, numbers_lists[1].split()))
    x_coord = []
    y_coord = []
    with open(path, 'r') as file:
        if file.closed:
            print("Error opening file.")
        else:
            reading_coords = False
            for line in file:
                line = line.strip()
                if line == "NODE_COORD_SECTION":
                    reading_coords = True
                    continue
                if not reading_coords:
                    continue
                tokens = line.split()
                if len(tokens) >= 3:
                    x_coord.append(int(tokens[1]))
                    y_coord.append(int(tokens[2]))

    node_positions = {(i+1): (x, y) for i,(x, y) in enumerate(zip(x_coord, y_coord))}

    G1 = nx.Graph()
    G1.add_nodes_from(cycle1_nodes)
    G1.add_edges_from(zip(cycle1_nodes, cycle1_nodes[1:] + [cycle1_nodes[0]]))
    nx.set_node_attributes(G1, node_positions, 'pos') 

    G2 = nx.Graph()
    G2.add_nodes_from(cycle2_nodes)
    G2.add_edges_from(zip(cycle2_nodes, cycle2_nodes[1:] + [cycle2_nodes[0]]))
    nx.set_node_attributes(G2, node_positions, 'pos')  

    plt.figure(figsize=(20, 20))

    pos1 = nx.get_node_attributes(G1, 'pos')
    nx.draw(G1, pos1, with_labels=True, node_color='skyblue', edge_color='skyblue', node_size=400, label='TSP Cycle 1')

    pos2 = nx.get_node_attributes(G2, 'pos')
    nx.draw(G2, pos2, with_labels=True, node_color='salmon', edge_color='salmon', node_size=400, label='TSP Cycle 2')

    plt.title('TSP Cycles')
    plt.legend()
    plt.savefig('graph.png')
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python generate_visualization.py <file_path> <cycles>")
    else:
        path = sys.argv[1]
        cycles = sys.argv[2]
        visualize_tsp_cycles(path, cycles)
