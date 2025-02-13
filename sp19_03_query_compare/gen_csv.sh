#!/bin/bash
#
# Generate CSV Data
#
# Description:
#     Generates CSV files based on results generated by "bench.sh". Run that
#     first. The CSV files will be valid for importing into graphing software
#     or even spreadsheet software such as Microsoft Excel. It will NOT
#     generate averages (well it may in the future I guess).
#
# Author:
#     Clara Nguyen & Rach Offutt
#

# -----------------------------------------------------------------------------
# 1. Setup                                                                 {{{1
# -----------------------------------------------------------------------------

# Colours
red=$(tput setaf 1)
green=$(tput setaf 2)
yellow=$(tput setaf 3)
normal=$(tput sgr 0)

# -----------------------------------------------------------------------------
# 2. Process                                                               {{{1
# -----------------------------------------------------------------------------

	# -------------------------------------------------------------------------
	# 2.1. Check Directory                                                 {{{2
	# -------------------------------------------------------------------------

	printf "[${yellow}CHECK${normal}] %-40s" "\"results\" directory..."

	if [ ! -e "results" ]; then
		printf "[${red}FAILED${normal}]\n"
		printf \
			"%s %s\n" \
			"You have not run \"bench.sh\", or simply did not generate any" \
			"results. Run \"bench.sh\"."
		exit 1
	else
		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.2. Check Each Test                                                 {{{2
	# -------------------------------------------------------------------------

	function chk_dir {
		printf \
			"  [ ${yellow}DIR${normal} ] %-38s" \
			"\"results/$1\" directory..."

		if [ ! -e "results/$1" ]; then
			printf "[ ${yellow}SKIP${normal} ]\n"
		else
			printf "[  ${green}OK${normal}  ]\n"
		fi
	}

	function chk_dir2 {
		printf \
			"  [ ${yellow}DIR${normal} ] %-38s" \
			"\"results/$1\" directory..."

		if [ ! -e "results/$1" ]; then
			printf "[ ${green}MAKE${normal} ]\n"
		else
			printf "[${green}OVERWR${normal}]\n"
		fi
	}

	# Check each directory
	chk_dir  "mysql"
	chk_dir  "mysql_cmd"
	chk_dir  "redis"
	chk_dir  "redis_cmd"
	chk_dir2 "csv"

	# Recreate "results/csv", or make it for the first time.
	if [ -e "results/csv" ]; then
		rm -rf "results/csv"
	fi

	mkdir "results/csv"

	# -------------------------------------------------------------------------
	# 2.3. "mysql" via nodejs                                              {{{2
	# -------------------------------------------------------------------------

	if [ -e "results/mysql" ]; then
		printf "[ ${green}GEN${normal} ] %-40s" "mysql via nodejs..."

		# Let's go into the directory
		cd "results/mysql"
		mkdir "tmp"

		for F in *.txt; do
			cat "$F" | sed 's/.* \(.*\)ms/\1/' > "tmp/$F"
		done

		# Use paste to make the CSV
		paste "tmp/"*".txt" -d "," > "../csv/mysql.csv"
		
		# Clean up and go back
		rm -rf "tmp"
		cd - > /dev/null

		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.4. "mysql" via Command Line                                        {{{2
	# -------------------------------------------------------------------------

	if [ -e "results/mysql_cmd" ]; then
		printf "[ ${green}GEN${normal} ] %-40s" "mysql via Command Line..."

		# Let's go into the directory
		cd "results/mysql_cmd"
		mkdir "tmp"

		for F in *.txt; do
			cat "$F" | sed 's/.*(\(.*\) sec)/\1/' > "tmp/$F"
		done

		# Use paste to make the CSV
		paste "tmp/"*".txt" -d "," > "../csv/mysql_cmd.csv"
		
		# Clean up and go back
		rm -rf "tmp"
		cd - > /dev/null

		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.5. "redis" via Node.JS                                             {{{2
	# -------------------------------------------------------------------------

	if [ -e "results/redis" ]; then
		printf "[ ${green}GEN${normal} ] %-40s" "redis via nodejs..."

		# Let's go into the directory
		cd "results/redis"
		mkdir "tmp"

		for F in *.txt; do
			cat "$F" | sed 's/.* \(.*\)ms/\1/' > "tmp/$F"
		done

		# Use paste to make the CSV
		paste "tmp/"*".txt" -d "," > "../csv/redis.csv"
		
		# Clean up and go back
		rm -rf "tmp"
		cd - > /dev/null

		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.6. "redis" via Command Line                                        {{{2
	# -------------------------------------------------------------------------

	if [ -e "results/redis_cmd" ]; then
		printf "[ ${green}GEN${normal} ] %-40s" "redis via Command Line..."

		# Let's go into the directory
		cd "results/redis_cmd"

		# Use paste to make the CSV
		paste *".txt" -d "," > "../csv/redis_cmd.csv"
		
		# Clean up and go back
		cd - > /dev/null

		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.7. "mysql" via Node.JS (Find)                                      {{{2
	# -------------------------------------------------------------------------

	if [ -e "results/mysql_find" ]; then
		printf "[ ${green}GEN${normal} ] %-40s" "mysql via nodejs (Find)..."

		# Let's go into the directory
		cd "results/mysql_find"
		mkdir "tmp"

		for F in *.txt; do
			cat "$F" | sed 's/.* \(.*\)ms/\1/' > "tmp/$F"
		done

		# Use paste to make the CSV
		paste "tmp/"*".txt" -d "," > "../csv/mysql_find.csv"
		
		# Clean up and go back
		rm -rf "tmp"
		cd - > /dev/null

		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.8. "mysql" via Command Line (Find)                                 {{{2
	# -------------------------------------------------------------------------

	if [ -e "results/mysql_cmd_find" ]; then
		printf "[ ${green}GEN${normal} ] %-40s" "mysql via Command Line (Find)..."

		# Let's go into the directory
		cd "results/mysql_cmd_find"
		mkdir "tmp"

		for F in *.txt; do
			cat "$F" | sed 's/.*(\(.*\) sec)/\1/' > "tmp/$F"
		done

		# Use paste to make the CSV
		paste "tmp/"*".txt" -d "," > "../csv/mysql_cmd_find.csv"
		
		# Clean up and go back
		rm -rf "tmp"
		cd - > /dev/null

		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.9. "redis" via Node.JS (Find)                                      {{{2
	# -------------------------------------------------------------------------

	if [ -e "results/redis_find" ]; then
		printf "[ ${green}GEN${normal} ] %-40s" "redis via nodejs (Find)..."

		# Let's go into the directory
		cd "results/redis_find"
		mkdir "tmp"

		for F in *.txt; do
			cat "$F" | sed 's/.* \(.*\)ms/\1/' > "tmp/$F"
		done

		# Use paste to make the CSV
		paste "tmp/"*".txt" -d "," > "../csv/redis_find.csv"
		
		# Clean up and go back
		rm -rf "tmp"
		cd - > /dev/null

		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.10. "redis" via Command Line (Find)                                {{{2
	# -------------------------------------------------------------------------

	if [ -e "results/redis_cmd_find" ]; then
		printf "[ ${green}GEN${normal} ] %-40s" "redis via Command Line (Find)..."

		# Let's go into the directory
		cd "results/redis_cmd_find"

		# Use paste to make the CSV
		paste *".txt" -d "," > "../csv/redis_cmd_find.csv"
		
		# Clean up and go back
		cd - > /dev/null

		printf "[  ${green}OK${normal}  ]\n"
	fi

	# 2}}}
