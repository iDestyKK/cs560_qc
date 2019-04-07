#!/bin/bash
echo mkfs

for i in `seq 1 1000000`; do
	echo "mkdir test$i"
done
