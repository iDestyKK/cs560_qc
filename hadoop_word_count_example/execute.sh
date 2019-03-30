#!/bin/bash
#
# MapReduce Word Indexer Thing
#
# Description:
#     It's a thing that does a thing.
#
# Author:
#     Clara Nguyen & Rachel Offutt
#

# Argument check
if [ $# -eq 0 ]; then
	printf "Usage: %s file1 [file2 [file3...]]\n" "$0"
	exit 1
fi

# -----------------------------------------------------------------------------
# 1. Setup                                                                 {{{1
# -----------------------------------------------------------------------------

# Concat files into a single string
fstr=""
for f in $@; do
	bn=$(basename $f)
	if [ "$fstr" == "" ]; then
		fstr="/exp_data/$bn"
	else
		fstr="${fstr},/exp_data/${bn}"
	fi
done

# Configure hdfs so that we have a file to work with
sudo /usr/local/hadoop-2.7.6/bin/hadoop dfs -rm -r /exp_result
sudo /usr/local/hadoop-2.7.6/bin/hadoop dfs -rm -r /exp_data
sudo /usr/local/hadoop-2.7.6/bin/hadoop dfs -mkdir /exp_data

for f in $@; do
	bn=$(basename $f)
	sudo /usr/local/hadoop-2.7.6/bin/hadoop dfs -put "$f" "/exp_data/$bn"
done

# Run the experiment
sudo ./experiment.sh "$fstr" "/exp_result"

# Get the results back
rm -f results.txt
sudo /usr/local/hadoop-2.7.6/bin/hadoop dfs -get /exp_result/part-00000 results.txt

# Run the query application
python3 py/query.py results.txt
