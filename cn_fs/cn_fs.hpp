/*
 * CN_FS: Implementation file
 *
 * Description:
 *     Contains all functions ("mkfs", "open", etc) used to make CN_FS
 *     functional. Also contains all classes required.
 *
 * Author:
 *     Clara Nguyen
 */

#ifndef __CN_FS__
#define __CN_FS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

//Handy
#include "lib/handy/bstream.hpp"

//Utilities
#include "incl/utility.hpp"

using namespace std;
using namespace handy;

//Macros
#define _ARGS vector<string>

//Hard-coded values to satisfy assignment requirements
#define DISK_SIZE 104857600
#define SECT_SIZE 1024

namespace cn_fs {
	struct file {
		file();
		~file();

		void setup();

		bool in_use;
		size_t position;
		size_t size;
		size_t start_sector;
	};

	struct fs_header {
		unsigned int first_unused_sector;
		unsigned int root_sector;
		unsigned int S, D;
	};

	namespace func {
		int mkfs(_ARGS&);
		int open(_ARGS&);
		int read(_ARGS&);
		int write(_ARGS&);
		int seek(_ARGS&);
		int close(_ARGS&);
		int mkdir(_ARGS&);
		int rmdir(_ARGS&);
		int cd(_ARGS&);
		int ls(_ARGS&);
		int cat(_ARGS&);
		int tree(_ARGS&);
		int import(_ARGS&);
		int _export(_ARGS&);
	}
	namespace global {
		extern map<size_t, cn_fs::file> fd_dir;
	}
}

#endif
