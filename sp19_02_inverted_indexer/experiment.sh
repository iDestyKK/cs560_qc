#!/bin/bash

if [ $# -ne 2 ]; then
	printf "Usage: %s inputs output\n" "$0"
	exit 1
fi

HADOOP_DIR="/usr/local/hadoop-2.7.6"
BIN_DIR="${HADOOP_DIR}/bin"

${BIN_DIR}/hadoop jar \
	${HADOOP_DIR}/share/hadoop/tools/lib/hadoop-streaming-2.7.6.jar \
	-file    py/mapper.py       \
	-file    py/reducer.py      \
	-mapper  py/mapper.py       \
	-reducer py/reducer.py      \
	-input   "$1"               \
	-output  "$2"
