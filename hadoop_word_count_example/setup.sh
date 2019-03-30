#!/bin/bash

# Find Hadoop directories
printf "Finding directories... "

HADOOP_DIR="/usr/local/hadoop-2.7.6"
BIN_DIR="${HADOOP_DIR}/bin"

printf "Done\n"

# Create HDFS directory
printf "[HDFS] Making \"tmp_dat\"... "
sudo ${BIN_DIR}/hdfs dfs -mkdir /tmp_dat
sudo ${BIN_DIR}/hdfs dfs -chown iDestyKK /tmp_dat
printf "Done\n"

# Copy local data
printf "[HDFS] Copy test data over... "
${BIN_DIR}/hdfs dfs -put data/100-0.txt /tmp_dat
printf "Done\n"
