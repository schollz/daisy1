import matplotlib.pyplot as plt

# Read noise data from the file
with open("noise_data.txt", "r") as file:
    noise_data = [float(line.strip()) for line in file]

# Generate x-axis values (time)
sample_rate = 44100  # Sample rate used in the C++ program
num_samples = len(noise_data)
time_data = [i / sample_rate for i in range(num_samples)]  # Time in seconds

# Plot the noise data
plt.plot(time_data, noise_data)
plt.title("LFNoise2/LFNoise0 Plot")
plt.xlabel("Time (s)")
plt.ylabel("Noise Amplitude")
plt.show()
