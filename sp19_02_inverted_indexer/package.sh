#!/bin/bash
#
# cs560_qc - Programming Assignment 2 Submission Package Script
#
# Description:
#     Packages up files for submission. It'll create a "cs560_lab2.zip" file if
#     no arguments are specified. If an argument is specified, it'll name the
#     ZIP with that instead.
#
# Author:
#     Clara Nguyen
#

# -----------------------------------------------------------------------------
# Clean everything out and make a submit directory                         {{{1
# -----------------------------------------------------------------------------

printf "Setting up \"submit\" directory... "

rm -rf submit
mkdir submit

printf "Done!\n"

# -----------------------------------------------------------------------------
# Copy files over                                                          {{{1
# -----------------------------------------------------------------------------

printf "Copying files over... "

cp -r \
	copy.sh execute.sh experiment.sh patch.sh setup.sh \
	README.md \
	doc/pdf/"CS560 - Hadoop and MapReduce.pdf" \
	py \
	data \
	"submit"

printf "Done!\n"

# -----------------------------------------------------------------------------
# Package                                                                  {{{1
# -----------------------------------------------------------------------------

printf "Packaging...\n"

PKGN="cs560_lab2.tar"

# If an argument is specified, name it that instead.
if [ $# -eq 0 ]; then
	printf "  No filename specified. Defaulting to \"%s\"...\n" "${PKGN}"
else
	PKGN=$1
fi

printf "  TAR Exporting to \"%s\"...\n" "$PKGN"

current_directory=$(pwd)

cd submit

# Let TAR print out its output.
tar -cvf "${current_directory}/${PKGN}" * | sed 's/^/    /'

printf "Done!\n"
