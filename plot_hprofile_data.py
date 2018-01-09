#!/bin/python

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import argparse

def parsecli():
    parser = argparse.ArgumentParser(description="Visualize host-generated opencl_ciphers_improved_test profiler data")
    parser.add_argument('-w','--windowsize', help='Window size for running average', type=int, default=20)
    parser.add_argument('file', metavar='filename', help='Text file with the profiler data', type=str)
    return parser.parse_args()


class line_resizer:
    def __init__(self, ax, line, **kwargs):
        self.ax = ax
        fig = plt.gcf()
        self.trans = self.ax.transData.transform
        self.line = line
        self.lw = 1
        self.lw_data = kwargs.pop("linewidth", 1)
        self.ppd=72./fig.dpi
        self._resize()
        self.cid = self.ax.figure.canvas.mpl_connect('resize_event', self._resize)
        self.cid1 = self.ax.figure.canvas.mpl_connect("motion_notify_event", self._resize)
        self.cid2 = self.ax.figure.canvas.mpl_connect('button_release_event', self._resize)
    def _resize(self, event=None):
        lw =  ((self.trans((1,self.lw_data))-self.trans((0,0)))*self.ppd)[1]
        if lw != self.lw:
            self.line.set_linewidth(lw)
            self.ax.figure.canvas.draw_idle()
            self.lw = lw

def running_average(datay, datax, windowsize):
    timetick = (np.max(datax) - np.min(datax)) / datax.shape[0]
    return np.convolve(datay, np.ones((windowsize,))/windowsize, mode='valid')


def main(cli):
    raw_dataset = []
    pretty_dataset = []
    kernels_only_dataset = []
    input_only_dataset = []
    output_only_dataset = []
    optypes = []

    with open(cli.file, "r") as fp:
        lines = fp.read().split("\n")
        for line in lines:
            if len(line) == 0: continue
            (opname, skernid, sstart, send) = line.split(",")
            optypes.append(opname)
            raw_dataset.append((opname, int(skernid), int(sstart)/1000000, int(send)/1000000))

    relative_zero_t = min(raw_dataset[0][2], raw_dataset[0][3])
    for datum in raw_dataset:
        relative_zero_t = min([relative_zero_t, datum[2], datum[3]])

    optypes = sorted(list(set(optypes)))

    for datum in raw_dataset:
        datarow = [datum[1], optypes.index(datum[0]), datum[2]-relative_zero_t, datum[3]-relative_zero_t]
        pretty_dataset.append(datarow)
        if datum[0] == "kernel":
            kernels_only_dataset.append(datarow)
        elif datum[0] == "input":
            input_only_dataset.append(datarow)
        elif datum[0] == "output":
            output_only_dataset.append(datarow)

    arr = np.array(pretty_dataset)
    arr_kernels = np.array(kernels_only_dataset)
    arr_output = np.array(output_only_dataset)
    arr_input = np.array(input_only_dataset)

    colors = {0: 'red', 1: 'blue', 2: 'green'}
    color_mapper = np.vectorize(lambda x: colors.get(x % 3))


    # Top graph
    ax1 = plt.subplot(211)

    y_timeline = arr[:,0]
    c_timeline = arr[:,1]
    x1_timeline = arr[:,2]
    x2_timeline = arr[:,3]

    ax1.set_ylim(-0.5, np.max(y_timeline)+0.5)

    ax1.set_ylabel('Command queue')
    ax1.set_title('Timeline')

    lines = ax1.hlines(y_timeline, x1_timeline, x2_timeline, colors=color_mapper(c_timeline))
    r = line_resizer(ax1, lines, linewidth=0.98)

    handles = []
    for i,optype in enumerate(optypes):
        handles.append(mpatches.Patch(color=colors[i], label=optype))
    ax1.legend(handles=handles, loc='center right')

    # Bottom graph
    ax2 = plt.subplot(212, sharex=ax1)

    x_k_dur = (arr_kernels[:,3] + arr_kernels[:,2]) / 2
    k_durations = arr_kernels[:,3] - arr_kernels[:,2]
    y_k_dur = running_average(k_durations, x_k_dur, cli.windowsize)
    x_k_dur.resize(y_k_dur.shape)
    line_k_dur = ax2.plot(x_k_dur, y_k_dur, label="Average kernel duration")

    # compute average input duration
    x_input_dur = (arr_input[:,3] + arr_input[:,2]) / 2
    input_durations = arr_input[:,3] - arr_input[:,2]
    y_input_dur = running_average(input_durations, x_input_dur, cli.windowsize)
    x_input_dur.resize(y_input_dur.shape)

    # compute average output duration
    x_output_dur = (arr_output[:,3] + arr_output[:,2]) / 2
    output_durations = arr_output[:,3] - arr_output[:,2]
    y_output_dur = running_average(output_durations, x_output_dur, cli.windowsize)
    x_output_dur.resize(y_output_dur.shape)

    # interpolate average in, average out in x, and then sum them to get
    # the average io in x
    y_io_dur = np.interp(x_k_dur, x_input_dur, y_input_dur) + np.interp(x_k_dur, x_output_dur, y_output_dur)

    line_io_dur = ax2.plot(x_k_dur, y_io_dur, label="Average transfer duration")

    ax3 = ax2.twinx()
    k_over_io_ratio = y_k_dur / y_io_dur
    line_ratio = ax3.plot(x_k_dur, k_over_io_ratio, label="Ratio kernel/io duration", color="#2ca02c")

    ax2.set_xlabel('Time (ms)')
    ax2.set_ylabel('Time (ms)')
    ax3.set_ylabel('Ratio')
    ax2.legend(loc='upper left')
    ax3.legend(loc='upper right')
    plt.show()

if __name__ == "__main__":
    cli = parsecli()
    main(cli)
