#!/bin/bash 
 
# Define the combinations for each parameter 
nodes=(20 40 70 100) 
packets_per_sec=(300) 
node_speeds=(10) 
 
# Loop over all combinations 
for n in "${nodes[@]}"; do 
    for pps in "${packets_per_sec[@]}"; do 
        for speed in "${node_speeds[@]}"; do 
            # Construct the command with the current combination of parameters 
            command="./ns3 run \"scratch/manet-routing-compare --CSVfileName=output_result.csv --protocol=RAODV --flowMonitor=true --node=${n} --rate=${pps} --speed=${speed}\"" 
 
            # Print the command being run for logging purposes 
            echo "Running: $command" 
             
            # Execute the command 
            eval $command 


            # Optionally, append data to the CSV file manually (if ns3 doesn't do it)
            # You can check if results1.csv exists and append data or manage it through ns3
            #echo "Appending results for Node=${n}, Rate=${pps}, Speed=${speed}" >> results1.csv
        done 
    done 
done



nodes=(70) 
packets_per_sec=(300) 
node_speeds=(5 10 15 20) 
 
# Loop over all combinations 
for n in "${nodes[@]}"; do 
    for pps in "${packets_per_sec[@]}"; do 
        for speed in "${node_speeds[@]}"; do 
            # Construct the command with the current combination of parameters 
            command="./ns3 run \"scratch/manet-routing-compare --CSVfileName=output_result.csv --protocol=RAODV --flowMonitor=true --node=${n} --rate=${pps} --speed=${speed}\"" 
 
            # Print the command being run for logging purposes 
            echo "Running: $command" 
             
            # Execute the command 
            eval $command 


            # Optionally, append data to the CSV file manually (if ns3 doesn't do it)
            # You can check if results1.csv exists and append data or manage it through ns3
            #echo "Appending results for Node=${n}, Rate=${pps}, Speed=${speed}" >> results1.csv
        done 
    done 
done


nodes=(100) 
packets_per_sec=(100 200 300 400) 
node_speeds=(20) 
 
# Loop over all combinations 
for n in "${nodes[@]}"; do 
    for pps in "${packets_per_sec[@]}"; do 
        for speed in "${node_speeds[@]}"; do 
            # Construct the command with the current combination of parameters 
            command="./ns3 run \"scratch/manet-routing-compare --CSVfileName=output_result.csv --protocol=RAODV --flowMonitor=true --node=${n} --rate=${pps} --speed=${speed}\"" 
 
            # Print the command being run for logging purposes 
            echo "Running: $command" 
             
            # Execute the command 
            eval $command 


            # Optionally, append data to the CSV file manually (if ns3 doesn't do it)
            # You can check if results1.csv exists and append data or manage it through ns3
            #echo "Appending results for Node=${n}, Rate=${pps}, Speed=${speed}" >> results1.csv
        done 
    done 
done



nodes=(20 40 70 100) 
packets_per_sec=(300) 
node_speeds=(10) 
 
# Loop over all combinations 
for n in "${nodes[@]}"; do 
    for pps in "${packets_per_sec[@]}"; do 
        for speed in "${node_speeds[@]}"; do 
            # Construct the command with the current combination of parameters 
            command="./ns3 run \"scratch/manet-routing-compare --CSVfileName=output_result_aodv.csv --protocol=AODV --flowMonitor=true --node=${n} --rate=${pps} --speed=${speed}\"" 
 
            # Print the command being run for logging purposes 
            echo "Running: $command" 
             
            # Execute the command 
            eval $command 


            # Optionally, append data to the CSV file manually (if ns3 doesn't do it)
            # You can check if results1.csv exists and append data or manage it through ns3
            #echo "Appending results for Node=${n}, Rate=${pps}, Speed=${speed}" >> results1.csv
        done 
    done 
done


nodes=(70) 
packets_per_sec=(300) 
node_speeds=(5 10 15 20) 
 
# Loop over all combinations 
for n in "${nodes[@]}"; do 
    for pps in "${packets_per_sec[@]}"; do 
        for speed in "${node_speeds[@]}"; do 
            # Construct the command with the current combination of parameters 
            command="./ns3 run \"scratch/manet-routing-compare --CSVfileName=output_result_aodv.csv --protocol=AODV --flowMonitor=true --node=${n} --rate=${pps} --speed=${speed}\"" 
 
            # Print the command being run for logging purposes 
            echo "Running: $command" 
             
            # Execute the command 
            eval $command 


            # Optionally, append data to the CSV file manually (if ns3 doesn't do it)
            # You can check if results1.csv exists and append data or manage it through ns3
            #echo "Appending results for Node=${n}, Rate=${pps}, Speed=${speed}" >> results1.csv
        done 
    done 
done


nodes=(100) 
packets_per_sec=(100 200 300 400) 
node_speeds=(20) 
 
# Loop over all combinations 
for n in "${nodes[@]}"; do 
    for pps in "${packets_per_sec[@]}"; do 
        for speed in "${node_speeds[@]}"; do 
            # Construct the command with the current combination of parameters 
            command="./ns3 run \"scratch/manet-routing-compare --CSVfileName=output_result_aodv.csv --protocol=AODV --flowMonitor=true --node=${n} --rate=${pps} --speed=${speed}\"" 
 
            # Print the command being run for logging purposes 
            echo "Running: $command" 
             
            # Execute the command 
            eval $command 


            # Optionally, append data to the CSV file manually (if ns3 doesn't do it)
            # You can check if results1.csv exists and append data or manage it through ns3
            #echo "Appending results for Node=${n}, Rate=${pps}, Speed=${speed}" >> results1.csv
        done 
    done 
done