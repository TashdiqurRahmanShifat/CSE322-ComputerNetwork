#!/bin/bash

# Specify the folder containing the files
#folder_path="path_to_your_folder"
folder_path="."

# Prefix to be replaced and the new prefix
old_prefix="TcpVariantsComparison"
new_prefix="TcpVariantsComparisonTcpScalable"

# Find and rename files with the old prefix in their names
find "$folder_path" -type f -name "$old_prefix*" | while read old_file_path; do
    # Get the new filename by replacing the old prefix with the new prefix
    new_file_path="${old_file_path/$old_prefix/$new_prefix}"
    
    # Rename the file
    mv "$old_file_path" "$new_file_path"
    echo "Renamed: $old_file_path -> $new_file_path"
done

echo "All files have been renamed."
