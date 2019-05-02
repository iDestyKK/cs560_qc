/*
 * Key/Value Input Generator
 *
 * Description:
 *     Generates key/value pairs. Simple. Keys are generated randomly, and are
 *     guaranteed to be unique until an overflow occurs with unsigned long
 *     longs.
 *
 *     Since the benchmarking doesn't rely on this program, we aren't relying
 *     on speed. But it's "good enough".
 *
 * Synopsis:
 *     ./gen_input amount seed
 *
 * Error Codes:
 *     0 - Successful run.
 *     1 - Incorrect arguments.
 *     2 - Calloc failure.
 *
 * Author:
 *     Clara Nguyen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int       uint;
typedef unsigned long long u64;

// ----------------------------------------------------------------------------
// 1. Helper Functions                                                     {{{1
// ----------------------------------------------------------------------------

/*
 * rand_range
 *
 * Returns a random number in the range of [left, right). If right is less than
 * left, then it is [right, left).
 */

int rand_range(int l, int r) {
	return (r < l)
		? r + (rand() % (l - r))
		: l + (rand() % (r - l));
}

/*
 * fisher_yates
 *
 * Applies a shuffle on an array via the Fisher-Yates shuffling algorithm.
 * There are a few flaws. But it's "good enough".
 */

void fisher_yates(u64 *vec, uint size) {
	uint i, j;
	u64  k;
	for (i = 1; i < size; i++) {
		j = rand() % i;

		//Swap
		k = vec[j];
		vec[j] = vec[i];
		vec[i] = k;
	}
}

// ----------------------------------------------------------------------------
// 2. Main Function                                                        {{{1
// ----------------------------------------------------------------------------

main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s amount seed\n", argv[0]);
		exit(1);
	}

	uint i, j, k;
	u64 *keys;
	u64  l;

	i = 0;
	j = atoi(argv[1]);
	k = atoi(argv[2]);
	l = rand() % j;

	//Set the seed.
	srand(k);

	//Make and assign a "keys" vector via accumulation.
	keys = (u64 *) calloc(j, sizeof(u64));

	if (keys == NULL) {
		fprintf(stderr, "[FATAL] Failure with calloc().\n");
		exit(2);
	}

	for (i = 0; i < j; i++) {
		keys[i] = l;
		l += 1 + (rand() % j);
	}

	//Shuffle the unique keys.
	fisher_yates(keys, j);

	//Generate random key/value pairs
	for (i = 0; i < j; i++) {
		printf(
			"%llu %d\n",
			keys[i],
			rand_range(0, 1 + (rand() % j))
		);
	}

	exit(0);
}
