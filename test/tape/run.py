import sys
import matplotlib.pyplot as plt


def plot_monotonic_sequences():
    # Read data from stdin
    data = sys.stdin.read().strip().splitlines()

    # Parse the data into two columns
    col1 = []
    col2 = []
    for line in data:
        parts = line.split()
        if len(parts) == 2:
            col1.append(float(parts[0]))
            col2.append(float(parts[1]))

    # Find and plot monotonically increasing sequences
    start_index = 0
    for i in range(1, len(col1)):
        if col1[i] < col1[i - 1]:
            if start_index < i - 1:
                plt.plot(col1[start_index:i], col2[start_index:i])
            start_index = i
    # Plot the last sequence if it's increasing
    if start_index < len(col1) - 1:
        plt.plot(col1[start_index:], col2[start_index:])

    plt.xlabel("First Column")
    plt.ylabel("Second Column")
    plt.title("Monotonically Increasing Sequences")
    plt.show()


if __name__ == "__main__":
    plot_monotonic_sequences()
