import os
import numpy as np
import shutil

CYCLES_DIR = "../cycles"
PLOTS_DIR = "../plots"

def choose_best():
    for filename in os.listdir(CYCLES_DIR):
        if filename.endswith(".txt"):
            parts = filename.split("_")
            algo, instance = parts[0],  parts[1].split(".")[0]
            
            path = os.path.join(CYCLES_DIR, filename)
            data = np.loadtxt(path)
            idx = np.argmin(data)
            
            plot_filename = f"{algo}_{instance}_{idx + 1}.png"
            src_path = os.path.join(PLOTS_DIR, plot_filename)
            dst_path = os.path.join(PLOTS_DIR, f"BEST_{algo}_{instance}.png")
            shutil.copyfile(src_path, dst_path)

if __name__ == "__main__":
    choose_best()
