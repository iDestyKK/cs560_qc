#!/bin/bash

# Argument check
if [ $# -ne 2 ]; then
	printf "Usage: copy.sh username@address private_key\n"
	exit 1
fi

# Setup
CONN="$1"

# Check if key exists
if [ ! -e "$2" ]; then
	printf "[FATAL] Key file \"%s\" doesn't exist.\n" "$2"
	exit 2
fi

# Copy
ssh -i "$2" "${1}" "mkdir ~/mapreduce; chmod 700 ~/mapreduce"
scp -i "$2" -r py data experiment.sh setup.sh patch.sh execute.sh "${1}:~/mapreduce"
