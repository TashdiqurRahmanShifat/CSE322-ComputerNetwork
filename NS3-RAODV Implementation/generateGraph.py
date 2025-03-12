import pandas as pd
import matplotlib.pyplot as plt
import os

# File names
aodv_file = "output_result_aodv.csv"
raodv_file = "output_result.csv"

# Read data from CSV files
aodv_df = pd.read_csv(aodv_file)
raodv_df = pd.read_csv(raodv_file)

# Add a Protocol column to distinguish between AODV and RAODV
aodv_df['Protocol'] = 'AODV'
raodv_df['Protocol'] = 'RAODV'

# Combine the dataframes
combined_df = pd.concat([aodv_df, raodv_df], ignore_index=True)

# Function to generate and save graphs into separate folders
def generate_and_save_graphs():
    # Define output directories
    output_dirs = ["graphs_set1", "graphs_set2", "graphs_set3"]
    for output_dir in output_dirs:
        os.makedirs(output_dir, exist_ok=True)  # Create directories if they don't exist

    # 1. Fixed Nodes=70, Packet Rate=300, varying Node Speed
    for i, metric in enumerate(['Throughput', 'End to End Delay', 'Packet Delivery Ratio', 'Packet Drop Ratio']):
        plt.figure(figsize=(8, 6))
        for protocol in combined_df['Protocol'].unique():
            protocol_data = combined_df[
                (combined_df['Nodes'] == 70) & 
                (combined_df['Packet Transfer Rate(per s)'] == 300) &
                (combined_df['Protocol'] == protocol)
            ]
            plt.plot(protocol_data['NodeSpeed'], protocol_data[metric], marker='o', label=protocol, color='blue' if protocol == 'AODV' else 'green')
        plt.title(f'{metric} vs Node Speed (Nodes=70, Packet Rate=300)')
        plt.xlabel('Node Speed')
        plt.ylabel(metric)
        plt.legend()
        plt.grid()
        plt.savefig(f"graphs_set1/{metric}_vs_NodeSpeed_70_300.png")  # Save the graph
        plt.close()

    # 2. Fixed Packet Rate=300, Speed=10, varying Node Size
    for i, metric in enumerate(['Throughput', 'End to End Delay', 'Packet Delivery Ratio', 'Packet Drop Ratio']):
        plt.figure(figsize=(8, 6))
        for protocol in combined_df['Protocol'].unique():
            protocol_data = combined_df[
                (combined_df['NodeSpeed'] == 10) & 
                (combined_df['Packet Transfer Rate(per s)'] == 300) &
                (combined_df['Protocol'] == protocol)
            ]
            plt.plot(protocol_data['Nodes'], protocol_data[metric], marker='o', label=protocol, color='blue' if protocol == 'AODV' else 'green')
        plt.title(f'{metric} vs Node Size (Packet Rate=300, Speed=10)')
        plt.xlabel('Node Size')
        plt.ylabel(metric)
        plt.legend()
        plt.grid()
        plt.savefig(f"graphs_set2/{metric}_vs_NodeSize_300_10.png")  # Save the graph
        plt.close()

    # 3. Fixed Nodes=100, Speed=20, varying Packet Rate
    for i, metric in enumerate(['Throughput', 'End to End Delay', 'Packet Delivery Ratio', 'Packet Drop Ratio']):
        plt.figure(figsize=(8, 6))
        for protocol in combined_df['Protocol'].unique():
            protocol_data = combined_df[
                (combined_df['Nodes'] == 100) & 
                (combined_df['NodeSpeed'] == 20) &
                (combined_df['Protocol'] == protocol)
            ]
            plt.plot(protocol_data['Packet Transfer Rate(per s)'], protocol_data[metric], marker='o', label=protocol, color='blue' if protocol == 'AODV' else 'green')
        plt.title(f'{metric} vs Packet Rate (Nodes=100, Speed=20)')
        plt.xlabel('Packet Transfer Rate (per s)')
        plt.ylabel(metric)
        plt.legend()
        plt.grid()
        plt.savefig(f"graphs_set3/{metric}_vs_PacketRate_100_20.png")  # Save the graph
        plt.close()

# Call the function to generate and save graphs into separate folders
generate_and_save_graphs()
