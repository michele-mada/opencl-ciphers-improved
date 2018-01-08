#!/bin/python

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import argparse

def parsecli():
    parser = argparse.ArgumentParser(description="Visualize host-generated opencl_ciphers_improved_test profiler data")
    parser.add_argument('file', metavar='filename', help='Text file with the profiler data', type=str)
    return parser.parse_args()


def main(cli):
    raw_dataset = []
    pretty_dataset = []
    optypes = []

    with open(cli.file, "r") as fp:
        lines = fp.read().split("\n")
        for line in lines:
            if len(line) == 0: continue
            (opname, skernid, sstart, send) = line.split(",")
            optypes.append(opname)
            raw_dataset.append((opname, int(skernid), int(sstart), int(send)))

    relative_zero_t = min(raw_dataset[0][2], raw_dataset[0][3])
    for datum in raw_dataset:
        relative_zero_t = min([relative_zero_t, datum[2], datum[3]])

    optypes = list(set(optypes))

    for datum in raw_dataset:
        pretty_dataset.append([datum[1], optypes.index(datum[0]), datum[2]-relative_zero_t, datum[3]-relative_zero_t])

    arr = np.array(pretty_dataset)
    print(arr)

    colors = {0: 'red', 1: 'blue', 2: 'green'}

    color_mapper = np.vectorize(lambda x: colors.get(x % 3))
    # Plot a line for every line of data in your file
    y = arr[:,0]
    c = arr[:,1]
    x1 = arr[:,2]
    x2 = arr[:,3]

    plt.xlabel('Time (ns)')
    plt.ylabel('Command queue')
    plt.title('Profile')

    plt.hlines(y, x1, x2, colors=color_mapper(c), linewidth=50.0)
    #plt.barh(y, x2-x1, 0.9, x1, color=color_mapper(c))

    handles = []
    for i,optype in enumerate(optypes):
        handles.append(mpatches.Patch(color=colors[i], label=optype))
    plt.legend(handles=handles, loc='center right')

    plt.show()

if __name__ == "__main__":
    cli = parsecli()
    main(cli)
