import sys


def get_cycles(path):
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
    return node_positions