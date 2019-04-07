/*
 * Handy - File Management
 *
 * Description:
 *     Includes functions which involve file I/O and management.
 * 
 * Author:
 *     Clara Van Nguyen
 */

#ifndef __HANDY_FILEMGMT_CPP_HAN__
#define __HANDY_FILEMGMT_CPP_HAN__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "types.hpp"

namespace handy {
	namespace file {
		cn_bool file_exists   (const char*);
		size_t  get_file_size (const char*);

		cn_byte array_to_file (char**, const char*);
		char**  file_to_array (        const char*);
	}
}

#endif
