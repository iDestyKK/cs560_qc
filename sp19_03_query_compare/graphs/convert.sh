#!/bin/bash

for D in *; do
	# Proceed only if there are directories in question
	if [ ! -d "$D" ]; then
		continue;
	fi

	# Let's go into the directory
	cd "$D"

	# Clean up
	rm -rf "svg"
	mkdir "svg"

	for F in *".pdf"; do
		pdf2svg "$F" "svg/${F/pdf/svg}"
	done

	# Go back
	cd -
done
