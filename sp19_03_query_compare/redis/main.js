/*
 * COSC 560 (Spring 2019) - Query Compare / Redis Simulator
 *
 * Description:
 *     Simulates a Redis database adding in various keys from a text file. This
 *     is test program is made specifically for timing the speed of a Redis
 *     database vs others with raw inserts.
 *
 * Author:
 *     Clara Nguyen & Rach Offutt
 */

// ----------------------------------------------------------------------------
// 1. Setup                                                                {{{1
// ----------------------------------------------------------------------------

var fs    = require('fs'   );
var redis = require('redis');
var util  = require('util' );

// ----------------------------------------------------------------------------
// 2. Helper Functions                                                     {{{1
// ----------------------------------------------------------------------------

/*
 * file2pair
 *
 * Sets up key/value pairs for throwing into the Redis database. This is read
 * in from a file. Returns an array of pairs in the form of
 * [ [k, v], [k, v]... ].
 */

function file2pair(fpath) {
	let garbage = "";
	let stream  = undefined;
	let pairs   = [];
	let i;

	stream = fs.readFileSync(fpath).toString().split("\n");

	for (i = 0; i < stream.length; i++) {
		garbage = stream[i].split(" ");

		//EOF
		if (garbage.length != 2)
			break;

		pairs.push(+garbage[0]);
		pairs.push(+garbage[1]);
	}

	return pairs;
}

// ----------------------------------------------------------------------------
// 3. Main Function                                                        {{{1
// ----------------------------------------------------------------------------

//Let's do it C-style
async function main(argc, argv) {
	let conn  = undefined;
	let pairs = [];
	let start, end;

	//Argument Check
	if (argc != 2) {
		console.error("Usage: node main.js input_file");
		return 1;
	}

	//Read the file input in.
	pairs = file2pair(argv[1]);

	//Setup the database
	conn = redis.createClient();

	//Force redis to promise returning of all functions.
	conn.wipe         = util.promisify(conn.flushall);
	conn.query_single = util.promisify(conn.set);
	conn.query_multi  = util.promisify(conn.mset);

	//Wipe out the table... first off.
	await conn.wipe();

	start = new Date();

	await conn.query_multi(pairs);
	await conn.quit();

	end = new Date();

	console.log('Inserted ' + pairs.length + ' keys in ' + (end - start) + 'ms');

	return 0;
}

//Call main
//process.exit(
	main(
		process.argv.length - 1,
		process.argv.slice(1)
	)
//);
