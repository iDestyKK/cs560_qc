#!/bin/bash
#
# MapReduce Word Indexer Thing
#
# Description:
#     Runs the Hadoop MapReduce on specified files. Files read in are specified
#     in this script's arguments. After the MapReduce completes, a program
#     capable of running queries on the words that the MapReduce generated is
#     run. Type in any words and you will get information about them, whether
#     they exist in any of the files or not.
#
# Author:
#     Clara Nguyen & Rachel Offutt
#

# -----------------------------------------------------------------------------
# 0. Checks                                                                {{{1
# -----------------------------------------------------------------------------

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

# Specify what hadoop is
hadoop_cmd="sudo /usr/local/hadoop-2.7.6/bin/hadoop"

# Configure hdfs so that we have a file to work with
$hadoop_cmd dfs -rm -r /exp_result 2> /dev/null
$hadoop_cmd dfs -rm -r /exp_data   2> /dev/null
$hadoop_cmd dfs -mkdir /exp_data   2> /dev/null

# Copy over all files specified in the arguments list
for f in $@; do
	bn=$(basename $f)
	$hadoop_cmd dfs -put "$f" "/exp_data/$bn" 2> /dev/null
done

# -----------------------------------------------------------------------------
# 2. Execution                                                             {{{1
# -----------------------------------------------------------------------------

# Run the experiment
sudo ./experiment.sh "$fstr" "/exp_result"

# Get the results back
rm -f results.txt
$hadoop_cmd dfs -get /exp_result/part-00000 results.txt 2> /dev/null

# Run the query application
python3 py/query.py results.txt
