#!/bin/python

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import argparse

def parsecli():
    parser = argparse.ArgumentParser(description="Visualize host-generated opencl_ciphers_improved_test profiler data")
    parser.add_argument('-w','--windowsize', help='Window size for running average', type=int, default=10)
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

def running_average(datay, windowsize):
    return np.convolve(datay, np.ones((windowsize,))/windowsize, mode='valid')


def reject_outliers(datay, datax, m=2):
    # transform both x and y, in order to keep the synchronization
    new_datay = datay[abs(datay - np.mean(datay)) < m * np.std(datay)]
    new_datax = datax[abs(datay - np.mean(datay)) < m * np.std(datay)]
    return new_datay, new_datax



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
    
    if len(kernels_only_dataset) == 0:
        arr_kernels = np.copy(arr_output)

    colors = {0: 'red', 1: 'blue', 2: 'green'}
    color_mapper = np.vectorize(lambda x: colors.get(x % 3))


    # Top graph
    ax1 = plt.subplot(211)
    ax1.yaxis.grid()
    ax1.xaxis.grid()

    y_timeline = arr[:,0]
    c_timeline = arr[:,1]
    x1_timeline = arr[:,2]
    x2_timeline = arr[:,3]

    ax1.set_ylim(-2.5, np.max(y_timeline)+0.5)
    ax1.set_yticks(np.arange(0, np.max(y_timeline)+1, 1.0))

    ax1.set_ylabel('Command queue')
    ax1.set_title('Timeline')

    lines_q = ax1.hlines(y_timeline, x1_timeline, x2_timeline, colors=color_mapper(c_timeline))
    r = line_resizer(ax1, lines_q, linewidth=0.98)

    y_input = arr_input[:,0]
    c_input = arr_input[:,1]
    x1_input = arr_input[:,2]
    x2_input = arr_input[:,3]
    y_channel_input = np.repeat(-1, y_input.shape[0])
    y_output = arr_output[:,0]
    c_output = arr_output[:,1]
    x1_output = arr_output[:,2]
    x2_output = arr_output[:,3]
    y_channel_output = np.repeat(-1, y_output.shape[0])

    lines_chan_in = ax1.hlines(y_channel_input, x1_input, x2_input, colors=color_mapper(c_input))
    ri = line_resizer(ax1, lines_chan_in, linewidth=0.98)
    lines_chan_out = ax1.hlines(y_channel_output, x1_output, x2_output, colors=color_mapper(c_output))
    ro = line_resizer(ax1, lines_chan_out, linewidth=0.98)

    y_usage = arr_kernels[:,0]
    c_usage = arr_kernels[:,1]
    x1_usage = arr_kernels[:,2]
    x2_usage = arr_kernels[:,3]
    y_area_usage = np.repeat(-2, y_usage.shape[0])

    lines_u = ax1.hlines(y_area_usage, x1_usage, x2_usage, colors=color_mapper(c_usage))
    ru = line_resizer(ax1, lines_u, linewidth=0.98)

    handles = []
    for i,optype in enumerate(optypes):
        handles.append(mpatches.Patch(color=colors[i], label=optype))
    ax1.legend(handles=handles, loc='center right')

    # Bottom graph
    ax2 = plt.subplot(212, sharex=ax1)
    ax2.xaxis.grid()

    # plot the running average of kernel durations
    x_k_dur = (arr_kernels[:,3] + arr_kernels[:,2]) / 2  # x values are the centers of each kernel run
    k_durations = arr_kernels[:,3] - arr_kernels[:,2]  # duration = end-start
    y_k_dur = running_average(k_durations, cli.windowsize)
    x_k_dur.resize(y_k_dur.shape)  # the running average truncated some values from the y array, truncate x the same way
    line_k_dur = ax2.plot(x_k_dur, y_k_dur, label="Average kernel duration")

    # plot the running average of kernel Initiation Interval (the delay
    # between conecutive kernel starts
    x_k_period = (arr_kernels[:,3] + arr_kernels[:,2]) / 2
    k_start = np.sort(arr_kernels[:,2])  # sort the kernel start values, usually not needed
    k_start_delayed = np.roll(k_start.copy(), -1).copy()
    k_start_delayed.resize((k_start_delayed.shape[0]-1,))  # create a "delayed start" array, by shifting the start array 1 to the left and truncating the last item
    k_start.resize((k_start_delayed.shape[0],))  # also truncate the start array the same way
    x_k_period.resize((k_start_delayed.shape[0],))   # also truncate the x array the same way
    # there are huge peaks caused by the delay between test sequences
    # (when new memory is prepared).
    # filter out those unneeded values
    k_periods, x_k_period = reject_outliers(k_start_delayed - k_start, x_k_period)
    y_k_period = running_average(k_periods, cli.windowsize)
    x_k_period.resize(y_k_period.shape)
    line_k_period = ax2.plot(x_k_period, y_k_period, label="Average kernel II")

    # compute average input duration
    x_input_dur = (arr_input[:,3] + arr_input[:,2]) / 2
    input_durations = arr_input[:,3] - arr_input[:,2]
    y_input_dur = running_average(input_durations, cli.windowsize)
    x_input_dur.resize(y_input_dur.shape)

    # compute average output duration
    x_output_dur = (arr_output[:,3] + arr_output[:,2]) / 2
    output_durations = arr_output[:,3] - arr_output[:,2]
    y_output_dur = running_average(output_durations, cli.windowsize)
    x_output_dur.resize(y_output_dur.shape)

    # interpolate average in, average out in x, and then sum them to get
    # the average io in x
    y_io_dur = np.interp(x_k_dur, x_input_dur, y_input_dur) + np.interp(x_k_dur, x_output_dur, y_output_dur)

    line_io_dur = ax2.plot(x_k_dur, y_io_dur, label="Average transfer duration")

    ax3 = ax2.twinx()
    k_over_io_ratio = y_k_dur / y_io_dur
    line_ratio = ax3.plot(x_k_dur, k_over_io_ratio, label="Ratio kernel/io", color="#7e0eff")

    ax2.set_xlabel('Time (ms)')
    ax2.set_ylabel('Time (ms)')
    ax3.set_ylabel('Ratio')
    ax2.legend(loc='upper left')
    ax3.legend(loc='upper right')
    plt.show()

if __name__ == "__main__":
    cli = parsecli()
    main(cli)
