#!/bin/bash

for D in *; do
	# Proceed only if there are directories in question
	if [ ! -d "$D" ]; then
		continue;
	fi

	# Let's go into the directory
	cd "$D"

	# Clean up
	rm -rf "png"
	mkdir "png"

	for F in *".pdf"; do
		convert "svg/${F/pdf/svg}" "png/${F/pdf/png}"
	done

	# Go back
	cd -
done
