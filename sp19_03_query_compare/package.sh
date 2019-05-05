#!/bin/bash
#
# cs560_qc - Final Project Submission Package Script
#
# Description:
#     Packages up files for submission. It'll create a
#     "cs560_final_proj.tar.xz" file if no arguments are specified. If an
#     argument is specified, it'll name the tar.xz with that instead.
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
	c \
	graphs \
	mysql \
	mysql_find \
	redis \
	redis_find \
	csv \
	README.md \
	doc/pdf/"CS560 - Final Paper.pdf" \
	bench.sh \
	gen_csv.sh \
	"submit"

printf "Done!\n"

# -----------------------------------------------------------------------------
# Package                                                                  {{{1
# -----------------------------------------------------------------------------

printf "Packaging...\n"

PKGN="cs560_finalproj.tar.xz"

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
tar -cJvf "${current_directory}/${PKGN}" * | sed 's/^/    /'

printf "Done!\n"
