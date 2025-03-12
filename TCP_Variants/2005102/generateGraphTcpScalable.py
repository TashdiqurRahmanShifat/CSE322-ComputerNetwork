import pandas as pd
import matplotlib.pyplot as plt
import os

# List of extensions to process
extensions = ['cwnd', 'ssth', 'rtt', 'rto', 'next-tx', 'inflight', 'next-rx']

# File path format
file_template0 = 'TcpVariantsComparisonTcpScalable-flow0-{}.data'
file_template1 = 'TcpVariantsComparisonTcpScalable-flow1-{}.data'

# Loop through each extension
for ext in extensions:
    # Construct file paths for flow0 and flow1
    file0_path = file_template0.format(ext)
    file1_path = file_template1.format(ext)

    # Check if the files exist
    if not os.path.isfile(file0_path) or not os.path.isfile(file1_path):
        print(f"File not found for extension '{ext}' (Flow 0: {file0_path}, Flow 1: {file1_path})")
        continue

    try:
        # Reading the data files for the current extension
        data_flow0 = pd.read_csv(file0_path, sep='\s+', header=None, names=['Time', ext.upper()])
        data_flow1 = pd.read_csv(file1_path, sep='\s+', header=None, names=['Time', ext.upper()])

        # Check the columns to debug KeyError
        print(f"Columns in flow0 data for extension '{ext}': {data_flow0.columns}")
        print(f"Columns in flow1 data for extension '{ext}': {data_flow1.columns}")

        # Check if the data is empty or malformed
        if data_flow0.empty or data_flow1.empty:
            print(f"Warning: Empty data for extension '{ext}' (Flow 0: {file0_path}, Flow 1: {file1_path})")
            continue

        # Plot the data
        plt.figure(figsize=(12, 6))  # Set figure size
        plt.plot(data_flow1['Time'], data_flow1[ext.upper()], label=f'Flow 1 {ext}', color='red', linewidth=2.5)  # Flow 1
        plt.plot(data_flow0['Time'], data_flow0[ext.upper()], label=f'Flow 0 {ext}', color='black', linewidth=1.7)  # Flow 0

        plt.title(f'TCP Variants(TcpScalable): {ext.upper()} Over Time')
        plt.xlabel('Time (s)')
        plt.ylabel(f'{ext.upper()}')
        plt.legend()
        plt.grid()
        plt.tight_layout()

        # Save the plot
        save_path = f'TcpScalable_{ext.upper()}.png'
        plt.savefig(save_path)
        print(f"Plot saved: {save_path}")

        # Display the plot
        plt.show()

    except pd.errors.ParserError:
        print(f"Error reading data files for extension '{ext}' (Flow 0: {file0_path}, Flow 1: {file1_path})")
    except Exception as e:
        print(f"An unexpected error occurred for extension '{ext}': {e}")
