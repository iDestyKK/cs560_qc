#!/bin/bash
#
# Database Bench Script
#
# Description:
#     Puts various database software to the test with the insertion of several
#     keys. Each database has 1000 test cases that run 5 times (making for 5000
#     inserts). Each case increases in "difficulty" of 100 key-value pairs. So
#     by the 1000th test, there's 100000 key-value pairs being inserted into
#     the database at once.
#
#     This script relies heavily on "gen_input.c", which it compiles if
#     possible. This program generates random unique key-value pairs that can
#     be inserted into the databases. These keys are inserted at random, rather
#     than in order (Note that SQL uses B-Trees to store this data).
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
# 2. Checks                                                                {{{1
# -----------------------------------------------------------------------------

	# -------------------------------------------------------------------------
	# 2.1. gen_input                                                       {{{2
	# -------------------------------------------------------------------------

	printf "[${yellow}CHECK${normal}] %-40s" "\"c/gen_input\"..."

	if [ ! -e "c/gen_input" ]; then
		printf "[ ${yellow}MAKE${normal} ]\n"

		# Make. Assume it works (TODO: Don't make that assumption...)
		cd "c"
		make

		# Go back to original directory
		cd -
	else
		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.2. "node" Command                                                  {{{2
	# -------------------------------------------------------------------------

	# Check "node"
	printf "[${yellow}CHECK${normal}] %-40s" "\"node\" command..."
	if [ ! $(command -v node) ]; then
		printf "[${red}FAILED${normal}]\n"
		printf \
			"%s %s\n"\
			"\"node\" is not a valid command on this system." \
			"Do you have nodejs installed?"
		exit 1
	else
		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.3. "redis-cli" Command                                             {{{2
	# -------------------------------------------------------------------------

	# Check "node"
	printf "[${yellow}CHECK${normal}] %-40s" "\"redis-cli\" command..."
	if [ ! $(command -v redis-cli) ]; then
		printf "[${red}FAILED${normal}]\n"
		printf \
			"%s %s\n"\
			"\"redis-cli\" is not a valid command on this system." \
			"Do you have redis installed?"
		exit 1
	else
		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.4. "mysql/main.js"                                                 {{{2
	# -------------------------------------------------------------------------

	# Check "mysql/main.js"
	printf "[${yellow}CHECK${normal}] %-40s" "\"mysql/main.js\"..."
	if [ ! -e "mysql/main.js" ]; then
		printf "[ ${yellow}SKIP${normal} ]\n"
	else
		printf "[  ${green}OK${normal}  ]\n"
	fi

	# -------------------------------------------------------------------------
	# 2.5. "redis/main.js"                                                 {{{2
	# -------------------------------------------------------------------------

	# Check "redis/main.js"
	printf "[${yellow}CHECK${normal}] %-40s" "\"redis/main.js\"..."
	if [ ! -e "redis/main.js" ]; then
		printf "[ ${yellow}SKIP${normal} ]\n"
	else
		printf "[  ${green}OK${normal}  ]\n"
	fi

	# 2}}}

# -----------------------------------------------------------------------------
# 3. Execution                                                             {{{1
# -----------------------------------------------------------------------------

	# -------------------------------------------------------------------------
	# 3.1. Helper Functions                                                {{{2
	# -------------------------------------------------------------------------

	#
	# stress_out
	#
	# Description:
	#     Tests the specified JS_SCRIPT program with KEYS keys. Appends the
	#     results to the end of OUT_FILE.
	#
	# Syntax:
	#     stress_out JS_SCRIPT KEYS OUT_FILE
	#

	function stress_out {
		./c/gen_input $2 0 > OUT
		node "$1" OUT >> "$3"
		rm -f OUT
	}

	# -------------------------------------------------------------------------
	# 3.2. Setup                                                           {{{2
	# -------------------------------------------------------------------------

	# rm -rf "results"
	mkdir "results"

	# -------------------------------------------------------------------------
	# 3.3. mysql via Node.JS                                               {{{2
	# -------------------------------------------------------------------------

	TEST="mysql"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching MYSQL via nodejs..."

		for j in {1..5}; do
			for i in {1..1000}; do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i 1000

				let "k = i * 100"
				stress_out "mysql/main.js" $k "results/${TEST}/mysql_results.${j}.txt"
			done
			printf "\n"
		done
	fi

	# -------------------------------------------------------------------------
	# 3.4. mysql via Command Line                                          {{{2
	# -------------------------------------------------------------------------

	# mysql --user=cs560_usr --password=yes cs560_test

	TEST="mysql_cmd"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching MYSQL via command line..."

		for j in {1..5}; do
			for i in {1..1000}; do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i 1000

				let "k = i * 100"

				# Now let's go an extra step...
				./c/gen_input $k 0 \
					| sed 's/\(.*\) \(.*\)/(\1, \2)/' \
					| tr '\n' ' ' \
					| sed \
						-e 's/) (/), (/g' \
						-e 's/^/INSERT into test (t_key, t_value) VALUES /' \
						-e 's/ $/;\n/' \
					> OUT

				# Obliterate the current table
				echo "DELETE from test;" \
					| mysql --user=cs560_usr --password=yes cs560_test

				# Insert in.
				mysql -vvv --user=cs560_usr --password=yes cs560_test \
					<  OUT \
					|  grep 'sec' \
					>> "results/${TEST}/mysql_results.${j}.txt"

				# Clean up
				rm -f OUT
			done
			printf "\n"
		done
	fi

	# -------------------------------------------------------------------------
	# 3.5. redis via Node.JS Line                                          {{{2
	# -------------------------------------------------------------------------

	# -------------------------------------------------------------------------
	# 3.6. redis via Command Line                                          {{{2
	# -------------------------------------------------------------------------

	TEST="redis_cmd"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching REDIS via command line..."

		for j in {1..5}; do
			for i in {1..1000}; do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i 1000

				let "k = i * 100"

				echo "MULTI" > OUT
				echo "TIME" >> OUT

				# Now let's go an extra step...
				./c/gen_input $k 0 \
					| tr '\n' ' ' \
					| sed \
						-e 's/^/mset /' \
						-e 's/$/\n/' \
					>> OUT

				echo "TIME" >> OUT
				echo "EXEC" >> OUT

				# Obliterate the current table
				echo "flushall" | redis-cli > /dev/null

				# Insert in.
				redis-cli \
					< OUT \
					| tail -n 5 \
					| grep -v "OK" \
					| tr '\n' ' ' \
					| awk '{a = $1 + ($2 / 1000000); b = $3 + ($4 / 1000000); printf("%.09f\n", b - a); }' \
					>> "results/${TEST}/redis_results.${j}.txt"

				# Clean up
				rm -f OUT
			done
			printf "\n"
		done
	fi

	# 2}}}
