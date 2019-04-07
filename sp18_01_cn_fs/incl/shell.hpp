/*
 * CN_FS: Generic Shell Interface
 *
 * Description:
 *     A simple interface that allows the end user to interact with CN_FS from
 *     the command line.
 *
 * Author:
 *     Clara Nguyen
 */

#ifndef __CN_FS_SHELL__
#define __CN_FS_SHELL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "utility.hpp"
#include "../cn_fs.hpp"

using namespace std;

namespace cn_fs {
	class shell {
		public:
			shell();
			shell(const char*);

			void command_add(const char*, int (*)(vector<string>&));
			int interpret(const char*);
			int run();
		private:
			map<string, int (*)(vector<string>&)> cmd_map;
			string prompt;
	};
}

#endif
