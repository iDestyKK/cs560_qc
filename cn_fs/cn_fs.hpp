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
#include <stdint.h>
#include <time.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>

//Handy
#include "lib/handy/bstream.hpp"
#include "lib/handy/file.hpp"

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
	struct fs_header {
		uint32_t first_unused_sector;
		uint32_t root_sector;
		uint32_t size;
		uint32_t sect_size;
		uint32_t S, D;
	};

	enum mode {
		T_FILE,
		T_DIR
	};

	struct fs_stat {
		cn_fs::mode     st_mode; /* Tell whether it's a directory or a file */
		uint32_t        st_size; /* Size of file */
		struct timespec st_atim; /* Time of last access */
		struct timespec st_mtim; /* Time of last modification */
		struct timespec st_ctim; /* Time of last status change */
	};

	struct dir {
		dir(bstream&);
		void save();

		bstream* buf;
		map<string, unsigned int> files; /* name -> block ID */
	};

	struct file {
		//Constructors
		file();
		~file();

		//Basic functions
		void setup(bstream&, unsigned int);
		void save();

		//Variables
		bool in_use;
		size_t position;
		size_t start_sector;

		cn_fs::fs_stat stat;
		bstream bytes;
		bstream* buf;
	};

	namespace func {
		namespace internal {
			void inject_file(bstream&, unsigned int, cn_fs::mode);
			void resize_file(bstream&, unsigned int, uint32_t);
			void remove_file(bstream&, unsigned int);
			void tree_print(unsigned int);
		}

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

		//Extra?
		//int lsof(_ARGS&);
		//int stat(_ARGS&);
		//int pwd(_ARGS&);
		
		//Custom
		int dump(_ARGS&);
	}
	namespace global {
		extern uint32_t cur_dir;
		extern deque<uint32_t> dir_path;
		extern bstream* buf;
		extern map<size_t, cn_fs::file> fd_dir;
		extern int tree_level;
		extern vector<string> tree_char;
	}
}

#endif
