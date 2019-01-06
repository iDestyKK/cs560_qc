/*
 * CN_FS: Utility Library
 *
 * Description:
 *     Includes functions that are used generally throughout the entire program
 *     that don't belong in any other header file.
 *
 * Author:
 *     Clara Nguyen
 */

#ifndef __CN_FS_UTILITY__
#define __CN_FS_UTILITY__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <string>
#include <vector>

using namespace std;

namespace cn_fs {
	namespace util {
		void log_error(const char*, ...);
		void explode(vector<string>&, const char*);
		bool __on_escape(char);
	}
}

#endif
