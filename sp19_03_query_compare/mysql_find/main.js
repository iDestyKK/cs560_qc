/*
 * COSC 560 (Spring 2019) - Query Compare / MySQL Simulator
 *
 * Description:
 *     Simulates a MySQL database adding in various keys from a text file. This
 *     is test program is made specifically for timing the speed of a MySQL
 *     database vs others with raw inserts.
 *
 * Author:
 *     Clara Nguyen & Rach Offutt
 */

// ----------------------------------------------------------------------------
// 1. Setup                                                                {{{1
// ----------------------------------------------------------------------------

var fs    = require('fs'   );
var mysql = require('mysql');
var util  = require('util' );

// ----------------------------------------------------------------------------
// 2. Helper Functions                                                     {{{1
// ----------------------------------------------------------------------------

/*
 * mysql_manual_setup
 *
 * Sets up mysql database manually. I'd rather this than make assumptions.
 */

async function mysql_manual_setup() {
	let conn = undefined;

	conn = await mysql.createConnection({
		host: "localhost",
		user: "cs560_usr",
		password: "yes",
		database: "cs560_test",
		multipleStatements: true
	});

	await conn.connect(function(e) {
		if (e)
			throw e;
	});

	return conn;
}

/*
 * file2pair
 *
 * Sets up key/value pairs for throwing into the SQL database. This is read in
 * from a file. Returns an array of pairs in the form of [ [k, v], [k, v]... ].
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

		pairs.push([
			+garbage[0],
			+garbage[1]
		]);
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
	if (argc != 3) {
		console.error("Usage: node main.js input_file n");
		return 1;
	}

	//Read the file input in.
	pairs = file2pair(argv[1]);

	//Setup the database
	//Assume it works (TODO: Don't make that assumption...)
	conn = await mysql_manual_setup();

	//Force mysql to promise returning of all functions.
	conn.query = util.promisify(conn.query);
	conn.end   = util.promisify(conn.end);

	//Construct the query.
	let query_str = "SELECT * FROM test WHERE t_key IN (";

	for (let i = 0; i < pairs.length && i < +(argv[2]); i++) {
		query_str += pairs[i][0];
		if (i < pairs.length - 1 && i < +(argv[2] - 1))
			query_str += ", ";
	}
	query_str += ");";

	start = new Date();

	//Put some shit in.
	await conn.query(query_str);
	/*
	for (let i = 0; i < pairs.length; i++) {
		await conn.query(
			'INSERT into test (t_key, t_value) VALUES (' + pairs[i][0] + ', ' + pairs[i][1] + ');'
		);
	}
	*/

	//Kill the connection
	conn.end();

	end = new Date();

	console.log('Found ' + +(argv[2]) + ' keys in ' + (end - start) + 'ms');

	return 0;
}

//Call main
//process.exit(
	main(
		process.argv.length - 1,
		process.argv.slice(1)
	)
//);
