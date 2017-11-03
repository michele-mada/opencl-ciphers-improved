import matplotlib.pyplot as plt
import sys
import numpy as np

repetitions = 100
convert_to_megs = 1.0/1048576

with open(sys.argv[1] or "autotune.txt", "r") as fp:
    all_data = np.loadtxt(fp, delimiter="\t", skiprows=1)

block_sizes = all_data[:,0]
run_times = all_data[:,1]

throughput = (block_sizes * repetitions * convert_to_megs) / run_times
best_throughput = max(throughput)
best_blocksize = block_sizes[np.where(throughput == best_throughput)[0]]

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

plt.annotate(
    "Best throughput @ block_size=%uMB" % int(best_blocksize * convert_to_megs),
    xy=(best_blocksize * convert_to_megs, best_throughput), xytext=(-20, 20),
    textcoords='offset points', ha='left', va='bottom',
    bbox=dict(boxstyle='round,pad=0.5', fc='yellow', alpha=0.5),
    arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))

plt.show()
