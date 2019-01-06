#include "cn_fs.hpp"

namespace cn_fs {
	file::file() {
		in_use = false;
		position = 0;
		size = 0;
		start_sector = 0;
	}

	file::~file() {
		if (!in_use)
			return;

		printf("removed\n");
	}

	void file::setup() {
		in_use = true;
	}

	namespace func {
		/*
		 * cn_fs::func::mkfs
		 * 
		 * Description:
		 *     Formats the disk for use with CN_FS.
		 *
		 * Author Notes:
		 *     There are extra optional parameters in that allow for controlled
		 *     file size and sector size. This is mainly for debugging and
		 *     flexibility in the future.
		 *
		 * CN_FS Syntax:
		 *     mkfs [disk_size sect_size]
		 */

		int mkfs(_ARGS& args) {
			size_t disk_size, sect_size;

			//Figure out what size we need to use for the disk
			if (args.size() == 1) {
				//Use the default parameters
				disk_size = DISK_SIZE;
				sect_size = SECT_SIZE;
			}
			else
			if (args.size() == 3) {
				//Use a custom setup.
				disk_size = atoi(args[1].c_str());
				sect_size = atoi(args[2].c_str());

				//Quickly fail if the disk size isn't divisible by sector size.
				if (disk_size % sect_size) {
					cn_fs::util::log_error(
						"[MKFS][FATAL] Size error. %d is not divisible by %d."
						"\n",
						disk_size,
						sect_size
					);
					return 1;
				}
			}
			else {
				//You did it wrong.
				cn_fs::util::log_error(
					"Usage: mkfs [disk_size sect_size]\n"
					"Defaults to 100M disk size and 1024 sector size if "
					"nothing is specified.\n"
				);
				return 2;
			}

			//Assume we have correct information.
			//Make a new bstream and format it.
			//Have "BST_ZERO" so all empty sectors are memset to 0. This makes
			//it better for compression programs like GZ, BZ2, and XZ.
			bstream buffer(disk_size, BST_READ | BST_WRITE | BST_ZERO);

			//Format the "disk" to be ready for use.
			cn_fs::fs_header& header = buffer.at<cn_fs::fs_header>(0);

			//Set the first unused sector to sector S.
			header.first_unused_sector = 1;
			header.root_sector = 0xFFFFFFFF;

			//TODO: Create a directory and then dump to that sector.

			//Compute D and S.
			unsigned int N, D, S; //Nintendo DS
			N = ((disk_size / sect_size) + 4) * sizeof(unsigned int);
			S = 1 + ((N - 1) / sect_size);
			D = (N / sizeof(unsigned int)) - 4 - S;

			header.S = S;
			header.D = D;

			//Set up the sector pointers... in-place.
			unsigned int* sec_ptr =
				buffer.data<unsigned int>() + 2;

			for (int i = 0; i < D; i++)
				sec_ptr[i] = i + 1;

			//...And shuffle it (Fisher-Yates)
			for (int i = D - 1; i > 0; i--) {
				int j, k;
				j = rand() % (i + 1);

				//Swap
				k = sec_ptr[i];
				sec_ptr[i] = sec_ptr[j];
				sec_ptr[j] = k;
			}

			//Dump to disk
			FILE *fp;
			fp = fopen("disk.cndisk", "wb");
			fwrite(buffer.data(), sizeof(char), disk_size, fp);
			fclose(fp);

			//Return success code.
			return 0;
		}

		/*
		 * cn_fs::func::open
		 * 
		 * Description:
		 *     Open a file with the given "flag", return a file descriptor "fd"
		 *     associated with this file.
		 *
		 *     FLAGS:
		 *       1. "r"
		 *       2. "w"
		 *
		 *     The current file offset will be 0 when the file is opened. If a
		 *     file does not exist, and it is opened for "w", then it will be
		 *     created with a size of 0. This command should print an integer
		 *     as the fd of the file.
		 *
		 * Example:
		 *     open foo w
		 *     Shell returns SUCCESS, fd=5
		 *
		 * CN_FS Syntax:
		 *     open filename flag
		 */

		int open(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"open"
			);
			return 1;
		}

		/*
		 * cn_fs::func::read
		 * 
		 * Description:
		 *     Read "size" bytes from the file associated with fd, from the
		 *     current file offset. The current file offset will move forward
		 *     "size" bytes after read.
		 *
		 * Example:
		 *     read 5 10
		 *     Shell returns the contents of the file (assuming it has been
		 *     written)
		 *
		 * CN_FS Syntax:
		 *     read fd size
		 */

		int read(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"read"
			);
			return 1;
		}

		/*
		 * cn_fs::func::write
		 * 
		 * Description:
		 *     Write "string" into file associated with "fd", from current file
		 *     offset. The current file offset will move forward the size of
		 *     the string after write. Here "string" must be formatted as a
		 *     string. If the end of the file is reached, the size of the file
		 *     will be increased.
		 *
		 * Example:
		 *     write 5 "hello, world"
		 *
		 * CN_FS Syntax:
		 *     write fd string
		 */

		int write(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"write"
			);
			return 1;
		}

		/*
		 * cn_fs::func::seek
		 * 
		 * Description:
		 *     Move the current file offset associated with "fd" to a new file
		 *     offset at "offset". The "offset" means the number of bytes from
		 *     the beginning of the file.
		 *
		 * Example:
		 *     seek 5 10
		 *
		 * CN_FS Syntax:
		 *     seek fd offset
		 */

		int seek(_ARGS& args) {
			//Argument check
			if (args.size() != 3) {
				cn_fs::util::log_error("Usage: seek fd offset\n");
				return 1;
			}
			
			//Parse the arguments
			size_t fd     = atoi(args[1].c_str()),
			       offset = atoi(args[2].c_str());

			//Find the file with "fd"
			map<size_t, cn_fs::file>::iterator ii;
			ii = cn_fs::global::fd_dir.find(fd);

			if (ii != cn_fs::global::fd_dir.end()) {
				//Set the position
				ii->second.position = offset;
			}
			else {
				cn_fs::util::log_error(
					"[CLOSE][FATAL] File descriptor %d isn't open.\n",
					fd
				);
				return 1;
			}

			return 1;
		}

		/*
		 * cn_fs::func::close
		 * 
		 * Description:
		 *     Close the file associated with "fd".
		 *
		 * Example:
		 *     close 5
		 *
		 * CN_FS Syntax:
		 *     close fd
		 */

		int close(_ARGS& args) {
			//Argument check
			if (args.size() != 2) {
				cn_fs::util::log_error("Usage: close fd\n");
				return 1;
			}

			//Find the ID in the map and close that file.
			map<size_t, cn_fs::file>::iterator ii;
			size_t fd = atoi(args[1].c_str());

			//Find the file with "fd"
			ii = cn_fs::global::fd_dir.find(fd);

			if (ii == cn_fs::global::fd_dir.end()) {
				//Failure
				cn_fs::util::log_error(
					"[CLOSE][FATAL] File descriptor %d isn't open.\n",
					fd
				);
				return 1;
			}
			else {
				//Close it.
				cn_fs::global::fd_dir.erase(ii);
			}
		}

		/*
		 * cn_fs::func::mkdir
		 * 
		 * Description:
		 *     Create a sub-directory "dirname" under the current directory.
		 *
		 * Example:
		 *     mkdir foo
		 *
		 * CN_FS Syntax:
		 *     mkdir dirname [dirname [dirname [...]]]
		 */

		int mkdir(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"mkdir"
			);
			return 1;
		}

		/*
		 * cn_fs::func::rmdir
		 * 
		 * Description:
		 *     Remove the sub-directory "dirname".
		 *
		 * Example:
		 *     rmdir foo
		 *
		 * CN_FS Syntax:
		 *     rmdir dirname [dirname [dirname [...]]]
		 */

		int rmdir(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"rmdir"
			);
			return 1;
		}

		/*
		 * cn_fs::func::cd
		 * 
		 * Description:
		 *     Change the current directory to "dirname".
		 *
		 * Example:
		 *     cd ../../foo./bar
		 *
		 * CN_FS Syntax:
		 *     cd dirname
		 */

		int cd(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"cd"
			);
			return 1;
		}

		/*
		 * cn_fs::func::ls
		 * 
		 * Description:
		 *     Show the content of the current directory. No parameters need to
		 *     be supported.
		 *
		 * CN_FS Syntax:
		 *     ls
		 */

		int ls(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"ls"
			);
			return 1;
		}

		/*
		 * cn_fs::func::cat
		 * 
		 * Description:
		 *     Show the content of the file.
		 *
		 * Example:
		 *     cat foo
		 *
		 * CN_FS Syntax:
		 *     cat filename
		 */

		int cat(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"cat"
			);
			return 1;
		}

		/*
		 * cn_fs::func::tree
		 * 
		 * Description:
		 *     List the contents of the current directory in a tree-format. For
		 *     each file listed, its date file size should be included.
		 *
		 *     To understand this command better, you may refer to this command
		 *     output under the command line shell in a Windows system.
		 *
		 * CN_FS Syntax:
		 *     tree
		 */

		int tree(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"tree"
			);
			return 1;
		}

		/*
		 * cn_fs::func::import
		 * 
		 * Description:
		 *     Import a file from the host machine file system to the current
		 *     directory.
		 *
		 * Example:
		 *     import /d/foo.txt foo.txt
		 *
		 * CN_FS Syntax:
		 *     import srcname destname
		 */

		int import(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"import"
			);
			return 1;
		}

		/*
		 * cn_fs::func::_export
		 * 
		 * Description:
		 *     Export a file from the current directory to the host machine
		 *     file system.
		 *
		 * Example:
		 *     export foo.txt /d/foo.txt
		 *
		 * CN_FS Syntax:
		 *     export srcname destname
		 */

		int _export(_ARGS& args) {
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"export"
			);
			return 1;
		}
	}
	namespace global {
		map<size_t, cn_fs::file> fd_dir;
	}
}
