import numpy as np
from collections import Counter

input_file = "./retail.dat"
output_file = "./synthetic_retail.dat"

def read_sequences(filename):
    with open(filename, "r") as file:
        return [list(map(int, line.strip().split())) for line in file if line.strip()]

def write_sequences(filename, sequences):
    with open(filename, "w") as file:
        file.writelines(" ".join(map(str, seq)) + "\n" for seq in sequences)

def generate_all_sequences(num_samples, lengths, length_probs, numbers, number_probs):
    samples = []
    for idx in range(num_samples):
        length = np.random.choice(lengths, p=length_probs)
        # Sample more than needed, then take unique and trim
        seq = set()
        while len(seq) < length:
            # Sample in batches for speed
            candidates = np.random.choice(numbers, size=length, p=number_probs)
            seq.update(candidates)
        samples.append(sorted(list(seq)[:length]))
        if idx % 10_000 == 0:
            print(f"Generated {idx} samples")
    return samples

# Step 1: Read original data
data = read_sequences(input_file)

# Step 2: Compute distributions
sequence_lengths = [len(seq) for seq in data]
length_counter = Counter(sequence_lengths)
lengths = np.array(list(length_counter.keys()))
length_weights = np.array(list(length_counter.values()))
length_probs = length_weights / length_weights.sum()

number_counter = Counter(num for seq in data for num in seq)
numbers = np.array(list(number_counter.keys()))
number_weights = np.array(list(number_counter.values()))
number_probs = number_weights / number_weights.sum()

# Step 3: Generate new sequences
num_samples = 500_000
new_samples = generate_all_sequences(num_samples, lengths, length_probs, numbers, number_probs)

# Step 4: Write to file
write_sequences(output_file, new_samples)

print(f"Generated {num_samples} sequences and saved to {output_file}")
