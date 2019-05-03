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
bold=$(tput bold)

if [ $# -eq 3 ]; then
	CASES=$1
	INC=$2
	REPEAT=$3
elif [ $# -eq 0 ]; then
	CASES=1000
	INC=100
	REPEAT=5
else
	printf "Usage: %s [cases increment test_repeat]\n" "$0"
	exit 1
fi

# Print out our fancy prompt thing
printf "%s\n\n%s (%s, %s):\n\n" \
	"CS560 - Key-Value Comparison Bench Script" \
	"Bench Settings" \
	"${green}${bold}GREEN${normal}=above default" \
	"${yellow}${bold}YELLOW${normal}=under default"

printf "  %-20s     CONFIG      DEFAULT  \n" " "

# Cases Number
if [ $CASES -gt 1000 ]; then
	printf "  %-20s [ ${green}${bold}%8d${normal} ] [ %8d ]\n" \
		"Cases Num:" $CASES 1000
elif [ $CASES -lt 1000 ]; then
	printf "  %-20s [ ${yellow}${bold}%8d${normal} ] [ %8d ]\n" \
		"Cases Num:" $CASES 1000
else
	printf "  %-20s [ %8d ] [ %8d ]\n" \
		"Cases Num:" $CASES 1000
fi

# Key Incrementation
if [ $INC -gt 100 ]; then
	printf "  %-20s [ ${green}${bold}%8d${normal} ] [ %8d ]\n" \
		"Key Increment:" $INC 100
elif [ $INC -lt 100 ]; then
	printf "  %-20s [ ${yellow}${bold}%8d${normal} ] [ %8d ]\n" \
		"Key Increment:" $INC 100
else
	printf "  %-20s [ %8d ] [ %8d ]\n" \
		"Key Increment:" $INC 100
fi

# Test Repeats
if [ $REPEAT -gt 5 ]; then
	printf "  %-20s [ ${green}${bold}%8d${normal} ] [ %8d ]\n" \
		"Test Repeat:" $REPEAT 5
elif [ $REPEAT -lt 5 ]; then
	printf "  %-20s [ ${yellow}${bold}%8d${normal} ] [ %8d ]\n" \
		"Test Repeat:" $REPEAT 5
else
	printf "  %-20s [ %8d ] [ %8d ]\n" \
		"Test Repeat:" $REPEAT 5
fi

printf "\n"

# -----------------------------------------------------------------------------
# 2. Checks                                                                {{{1
# -----------------------------------------------------------------------------

	printf "Running Checks...\n"

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

	printf "\nRunning Benchmarks...\n"

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
	mkdir "results" 2> /dev/null

	# -------------------------------------------------------------------------
	# 3.3. mysql via Node.JS (Insert)                                      {{{2
	# -------------------------------------------------------------------------

	TEST="mysql"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching MYSQL via nodejs..."

		for j in $(seq 1 1 $REPEAT); do
			for i in $(seq 1 1 $CASES); do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i $CASES

				let "k = i * INC"
				stress_out "mysql/main.js" $k "results/${TEST}/mysql_results.${j}.txt"
			done
			printf "\n"
		done
	fi

	# -------------------------------------------------------------------------
	# 3.4. mysql via Command Line (Insert)                                 {{{2
	# -------------------------------------------------------------------------

	# mysql --user=cs560_usr --password=yes cs560_test

	TEST="mysql_cmd"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching MYSQL via command line..."

		for j in $(seq 1 1 $REPEAT); do
			for i in $(seq 1 1 $CASES); do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i $CASES

				let "k = i * INC"

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
	# 3.5. redis via Node.JS Line (Insert)                                 {{{2
	# -------------------------------------------------------------------------

	TEST="redis"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching REDIS via nodejs..."

		for j in $(seq 1 1 $REPEAT); do
			for i in $(seq 1 1 $CASES); do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i $CASES

				let "k = i * INC"
				stress_out "redis/main.js" $k "results/${TEST}/redis_results.${j}.txt"
			done
			printf "\n"
		done
	fi

	# -------------------------------------------------------------------------
	# 3.6. redis via Command Line (Insert)                                 {{{2
	# -------------------------------------------------------------------------

	TEST="redis_cmd"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching REDIS via command line..."

		for j in $(seq 1 1 $REPEAT); do
			for i in $(seq 1 1 $CASES); do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i $CASES

				let "k = i * INC"

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

	# -------------------------------------------------------------------------
	# 3.7. mysql via Node.JS (Find)                                        {{{2
	# -------------------------------------------------------------------------

	# Prepare MySQL Database for rapid finds.
	echo "DELETE from test;" > OUT

	./c/gen_input 100000 0 > V

	cat "V" \
		| sed 's/\(.*\) \(.*\)/(\1, \2)/' \
		| tr '\n' ' ' \
		| sed \
			-e 's/) (/), (/g' \
			-e 's/^/INSERT into test (t_key, t_value) VALUES /' \
			-e 's/ $/;\n/' \
		>> OUT

	# Set the table to have these values.
	mysql --user=cs560_usr --password=yes cs560_test < OUT

	# Now begin the real test
	TEST="mysql_find"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching MYSQL via nodejs (Find)..."

		for j in $(seq 1 1 $REPEAT); do
			for i in $(seq 1 1 $CASES); do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i $CASES

				let "k = i * INC"

				# No "stress_out" here. Just a simple command.
				node \
					"mysql_find/main.js" \
					V \
					$k \
					>> "results/${TEST}/mysql_results.${j}.txt"
			done
			printf "\n"
		done
	fi

	# -------------------------------------------------------------------------
	# 3.8. mysql via Command Line (Find)                                   {{{2
	# -------------------------------------------------------------------------

	# Assume V exists.
	TEST="mysql_cmd_find"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching MySQL (Find) via command line..."

		for j in $(seq 1 1 $REPEAT); do
			for i in $(seq 1 1 $CASES); do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i $CASES

				let "k = i * INC"

				# Take "K" keys out of the file and find them in the database.
				cat V \
					| head -n $k \
					| sed 's/\(.*\) \(.*\)/\1/' \
					| tr '\n' ',' \
					| sed \
						-e 's/,/, /g' \
						-e 's/, $/)\n/' \
						-e 's/^/SELECT * FROM test WHERE t_key IN (/' \
					| mysql -vvv --user=cs560_usr --password=yes cs560_test \
					|  grep 'sec' \
					>> "results/${TEST}/mysql_results.${j}.txt"

				# Clean up
				rm -f OUT
			done
			printf "\n"
		done
	fi

	# -------------------------------------------------------------------------
	# 3.9. redis via Node.JS (Find)                                        {{{2
	# -------------------------------------------------------------------------

	# Prepare Redis Database for rapid finds.
	echo "flushall" | redis-cli > /dev/null

	./c/gen_input 100000 0 > V

	cat V \
		| tr '\n' ' ' \
		| sed \
			-e 's/^/mset /' \
			-e 's/$/\n/' \
		| redis-cli > /dev/null

	# -------------------------------------------------------------------------
	# 3.10. redis via Command Line (Find)                                  {{{2
	# -------------------------------------------------------------------------

	TEST="redis_cmd_find"

	if [ ! -e "results/${TEST}" ]; then
		mkdir "results/${TEST}"
		j=0

		printf \
			"%-40s\n" \
			"[${green}BENCH${normal}] Benching REDIS via command line..."

		for j in $(seq 1 1 $REPEAT); do
			for i in $(seq 1 1 $CASES); do
				printf \
					"\r        %-33s[ %4d / %4d ]" \
					"- Run ${j}..." \
					$i $CASES

				let "k = i * INC"

				echo "MULTI" > OUT
				echo "TIME" >> OUT

				# Now let's go an extra step...
				cat V \
					| sed 's/\(.*\) \(.*\)/\1/' \
					| head -n $k \
					| tr '\n' ' ' \
					| sed \
						-e 's/^/mget /' \
						-e 's/$/\n/' \
					>> OUT

				echo "TIME" >> OUT
				echo "EXEC" >> OUT

				# Insert in.
				redis-cli \
					< OUT \
					> OUT_T

				# Construct the time string
				cat OUT_T | head -n 6 | tail -n 2 >  OUT_TI
				cat OUT_T             | tail -n 2 >> OUT_TI

				cat OUT_TI \
					| tr '\n' ' ' \
					| awk '{a = $1 + ($2 / 1000000); b = $3 + ($4 / 1000000); printf("%.09f\n", b - a); }' \
					>> "results/${TEST}/redis_results.${j}.txt"

				# Clean up
				rm -f OUT OUT_T OUT_TI
			done
			printf "\n"
		done
	fi

	# Clean up
	rm -f V

	# 2}}}
