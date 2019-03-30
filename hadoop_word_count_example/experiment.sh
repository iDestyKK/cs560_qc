#!/bin/bash

HADOOP_DIR="/usr/local/hadoop-2.7.6"
BIN_DIR="${HADOOP_DIR}/bin"

${BIN_DIR}/hadoop jar \
	${HADOOP_DIR}/share/hadoop/tools/lib/hadoop-streaming-2.7.6.jar \
	-file    py/mapper.py       \
	-file    py/reducer.py      \
	-mapper  py/mapper.py       \
	-reducer py/reducer.py      \
	-input   /tmp_dat/100-0.txt \
	-output  /tmp_dat/out.txt
