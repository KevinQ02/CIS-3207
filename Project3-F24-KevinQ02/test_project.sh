#!/bin/bash

# Compile the project
gcc -o project main.c utils.c file_manager.c logger.c -I.

# Run the project
./project

# List files in the unpacked directory
ls -l ./unpacked_files

# Check running processes
echo "Checking for any running worker processes:"
ps -ef | grep project

# Display the log file
cat log.txt
