#!/bin/python

import matplotlib.pyplot as plt
import sys
import numpy as np
import argparse

repetitions = 100
convert_to_megs = 1.0/1048576


def parsecli():
    parser = argparse.ArgumentParser(description="Visualize opencl_ciphers_improved_test tuning data")
    parser.add_argument('-s','--save', help='Save the graph as an image', action="store_true")
    parser.add_argument('-q','--quiet', help='Don\'t display/print anything', action="store_true")
    parser.add_argument('-X','--throughput', help='Just print the best throughput to stdout', action="store_true")
    parser.add_argument('-B','--blocksize', help='Just print the block size to stdout', action="store_true")
    parser.add_argument('file', metavar='filename', help='Text file with the tuning data', type=str, default="autotune.txt")
    return parser.parse_args()


def main(cli):
    with open(cli.file, "r") as fp:
        all_data = np.loadtxt(fp, delimiter="\t", skiprows=1)

    block_sizes = all_data[:,0]
    run_times = all_data[:,1]

    throughput = (block_sizes * repetitions * convert_to_megs) / run_times
    best_throughput = max(throughput)
    best_blocksize = block_sizes[np.where(throughput == best_throughput)[0]]

    if not cli.quiet:
        if cli.throughput:
            print("%.3f" % best_throughput, end="")
        elif cli.blocksize:
            print("%u" % int(best_blocksize * convert_to_megs), end="")
        else:
            print("Best throughput: %.3f MBs at blocksize: %uMB" % (best_throughput, int(best_blocksize * convert_to_megs)))

    if not (cli.throughput or cli.blocksize):
        fig, ax = plt.subplots()
        plt.plot(block_sizes * convert_to_megs, throughput)
        plt.margins(y=0.2)
        stride = 4
        if max(block_sizes * convert_to_megs) > 100: stride = 40
        elif max(block_sizes * convert_to_megs) > 1000: stride = 80
        ax.set_xticks(np.arange(0, max(block_sizes * convert_to_megs), stride))
        plt.xlabel('block size (MB)')
        plt.ylabel('throughput (MB/s)')
        plt.title('Throughput tuning')
        plt.grid(True)
        
        labelpos = "left"
        if best_blocksize > max(block_sizes)/2:
            labelpos = "right"

        plt.annotate(
            "Best: %.3fMB/s @ bs=%uMB" % (best_throughput, int(best_blocksize * convert_to_megs)),
            xy=(best_blocksize * convert_to_megs, best_throughput), xytext=(-20, 20),
            textcoords='offset points', ha=labelpos, va='bottom',
            bbox=dict(boxstyle='round,pad=0.5', fc='yellow', alpha=0.5),
            arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))

        figure = plt.gcf()
        if not cli.quiet:
            plt.show()
        if cli.save:
            plt.draw()
            figure.savefig(cli.file + ".png", dpi=150)
    
    
if __name__ == "__main__":
    cli = parsecli()
    main(cli)
