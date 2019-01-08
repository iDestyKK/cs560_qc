#include "cn_fs.hpp"

namespace cn_fs {
	// ------------------------------------------------------------------------
	// 1. Directory Struct                                                 {{{1
	// ------------------------------------------------------------------------

	/*
	 * dir constructor
	 *
	 * Description:
	 *     This struct is used only to store directory information. It takes an
	 *     isolated bstream from a file struct and parses it.
	 */

	dir::dir(bstream& _buf) {
		buf = &_buf;
		uint32_t pos = 0,
		         num,
		         tmp_sz,
		         tmp_sc;
		string tmp;

		//Get the number of files.
		num = buf->at<uint32_t>(0);
		pos += 4;

		//Now, for each file, create the map structure.
		for (int i = 0; i < num; i++) {
			//Get what sector this file starts at.
			tmp_sc = buf->at<uint32_t>(pos); pos += 4;

			//Get the number of characters in the string.
			tmp_sz = buf->at<uint32_t>(pos); pos += 4;

			//Set up the string, and copy all characters over.
			tmp.resize(tmp_sz);
			for (int j = 0; j < tmp_sz; j++) {
				tmp[j] = (*buf)[pos++];
			}

			//Push to the map
			files.insert(make_pair<string, unsigned int>(
				tmp,
				tmp_sc
			));
		}
	}

	void dir::save() {
		//Wipe out the buffer and recreate it.
		buf->resize(4);
		buf->at<uint32_t>(0) = files.size();
		
		uint32_t pos = 4;

		//Start pushing files to the buffer.
		map<string, unsigned int>::iterator ii;

		for (ii = files.begin(); ii != files.end(); ii++) {
			buf->resize(
				buf->size() +            /* Initial Size */
				(sizeof(uint32_t) * 2) + /* 2 uint32_t's */
				ii->first.size()         /* File name */
			);

			//Push sector number
			buf->at<uint32_t>(pos) = ii->second;
			pos += sizeof(uint32_t);

			//Push file name length
			buf->at<uint32_t>(pos) = ii->first.size();
			pos += sizeof(uint32_t);

			//Push filename
			memcpy(&buf->data()[pos], &ii->first[0], ii->first.size());
			pos += ii->first.size();
		}
	}

	// 1}}}

	// ------------------------------------------------------------------------
	// 2. File Struct                                                      {{{1
	// ------------------------------------------------------------------------

	file::file() {
		in_use = false;
		position = 0;
		start_sector = 0;
		buf = NULL;
	}

	file::~file() {
		if (!in_use)
			return;

		//TODO: Manage any pointers... if any (none yet)
	}

	void file::setup(bstream& _buf, unsigned int lba) {
		in_use = true;
		buf = &_buf;
		start_sector = lba;
		bool is_first_lba = true;

		//Get information on the disk
		cn_fs::fs_header &head = buf->at<cn_fs::fs_header>(0);
		unsigned int     *lbas = buf->data<unsigned int>() + 6;

		unsigned char* sect_bytes = buf->data<unsigned char>();
		sect_bytes += head.sect_size * (head.S + lba - 1);

		//Get the stat struct via reinterpret
		stat = *(cn_fs::fs_stat *) &sect_bytes[0];

		sect_bytes += sizeof(cn_fs::fs_stat);

		//Construct "bytes"
		bytes.resize(stat.st_size);
		position = 0;
		size_t batch_lim = head.sect_size - sizeof(cn_fs::fs_stat);
		size_t cur_sec = lba;

		while (position < stat.st_size) {
			//Copy the bytes in.
			memcpy(
				&bytes[position],
				&sect_bytes[0],
				(position + batch_lim < stat.st_size)
					? (batch_lim)
					: (stat.st_size - position)
			);

			//Increment the position
			position += batch_lim;

			//Increment the LBA and bytes pointers
			cur_sec = lbas[cur_sec - 1];

			sect_bytes = buf->data<unsigned char>();
			sect_bytes += head.sect_size * (head.S + cur_sec - 1);

			//We are no longer in a sector that has a stat struct.
			if (is_first_lba) {
				is_first_lba = false;
				batch_lim += sizeof(cn_fs::fs_stat);
			}
		}
	}

	void file::save() {
		//Update the size
		stat.st_size = bytes.size();

		//Adjust the FAT to account for the new size.
		cn_fs::func::internal::resize_file(
			*buf,
			start_sector,
			stat.st_size
		);

		//Set up data on the FAT.
		bool is_first_lba = true;

		//Get information on the disk
		cn_fs::fs_header &head = buf->at<cn_fs::fs_header>(0);
		unsigned int     *lbas = buf->data<unsigned int>() + 6;

		unsigned char* sect_bytes = buf->data<unsigned char>();
		sect_bytes += head.sect_size * (head.S + start_sector - 1);

		//Get the stat struct via reinterpret
		stat = *(cn_fs::fs_stat *) &sect_bytes[0];

		sect_bytes += sizeof(cn_fs::fs_stat);

		//Construct "bytes"
		position = 0;
		size_t batch_lim = head.sect_size - sizeof(cn_fs::fs_stat);
		size_t cur_sec = start_sector;

		while (position < stat.st_size) {
			//Copy the bytes in.
			memcpy(
				&sect_bytes[0],
				&bytes[position],
				(position + batch_lim < stat.st_size)
					? (batch_lim)
					: (stat.st_size - position)
			);

			//Increment the position
			position += batch_lim;

			//Increment the LBA and bytes pointers
			cur_sec = lbas[cur_sec - 1];

			sect_bytes = buf->data<unsigned char>();
			sect_bytes += head.sect_size * (head.S + cur_sec - 1);

			//We are no longer in a sector that has a stat struct.
			if (is_first_lba) {
				is_first_lba = false;
				batch_lim += sizeof(cn_fs::fs_stat);
			}
		}
	}

	// 1}}}

	namespace func {
		// --------------------------------------------------------------------
		// 3. Internal Functions                                           {{{1
		// --------------------------------------------------------------------
		
		/* 
		 * These functions are not intended to be used by the user. Instead,
		 * they are helper functions that the user functions do use. They CAN
		 * be called if an extension to CN_FS is done. Hence why they are not
		 * private.
		 */
		
		namespace internal {
			/*
			 * cn_fs::func::internal::inject_file                          {{{2
			 *
			 * Description:
			 *     Injects a file of whatever type (T_DIR/T_FILE) and updates
			 *     the File Allocation Table.
			 */

			void inject_file(bstream& buf, unsigned int lba, cn_fs::mode type) {
				//Grab the header and LBAs
				cn_fs::fs_header &head = buf.at<cn_fs::fs_header>(0);
				unsigned int     *lbas = buf.data<unsigned int>() + 6;

				//Update the next available LBA to the one in the array.
				head.first_unused_sector = lbas[lba - 1];

				//Set to 0 since (for now) it's the only sector for this file.
				lbas[lba - 1] = 0;

				//Jump to the LBA.
				unsigned char* bytes = buf.data<unsigned char>();
				bytes += head.sect_size * (head.S + lba - 1);

				//Get the stat struct via reinterpret
				cn_fs::fs_stat &st = *(cn_fs::fs_stat *) &bytes[0];

				//Set up the stat
				st.st_mode = type;
				st.st_size = 0;

				clock_gettime(CLOCK_REALTIME, &st.st_ctim);
				st.st_atim = st.st_mtim = st.st_ctim;
			}

			/*
			 * cn_fs::func::internal::resize_file                          {{{2
			 *
			 * Description:
			 *     Does what the name implies. It resizes the file at a target
			 *     LBA.
			 */

			void resize_file(bstream& buf, unsigned int lba, uint32_t nsz) {
				//Grab header and LBAs
				cn_fs::fs_header &head = buf.at<cn_fs::fs_header>(0);
				unsigned int     *lbas = buf.data<unsigned int>() + 6;

				//Grab the target LBA
				unsigned char* bytes = buf.data<unsigned char>();
				bytes += head.sect_size * (head.S + lba - 1);

				//Grab the stat struct
				cn_fs::fs_stat &st = *(cn_fs::fs_stat *) &bytes[0];
				size_t osz = st.st_size,
				       sect_count_new,
				       sect_count_old;

				//Go on and assume the new size.
				st.st_size = nsz;

				//Compensate for the stat struct being embedded in the file.
				osz += sizeof(cn_fs::fs_stat);
				nsz += sizeof(cn_fs::fs_stat);

				//Compute how many sectors are needed to hold the data.
				sect_count_old = 1 + ((osz - 1) / head.sect_size);
				sect_count_new = 1 + ((nsz - 1) / head.sect_size);

				if (sect_count_old == sect_count_new) {
					//Well that sucks.
					return;
				}
				else
				if (sect_count_new > sect_count_old) {
					//We're making new sectors. Skip to the last sector of the
					//file.
					unsigned int cs;
					for (cs = lba; ; cs = lbas[cs - 1]) {
						if (lbas[cs - 1] == cs - 1 || lbas[cs - 1] == 0)
							break;
					}

					//Align the LBAs.
					for (int i = sect_count_old; i < sect_count_new; i++) {
						lbas[cs - 1] = head.first_unused_sector;
						head.first_unused_sector = lbas[lbas[cs - 1] - 1];
						cs = lbas[cs - 1];
					}

					//Set the last sector's "next" to 0.
					lbas[cs - 1] = 0;
				}
				else {
					//We are losing sectors. Give them back to the LBA.
					unsigned int cs;
					deque<unsigned int> sectors;
					
					//Go to the last sector the file owns
					for (cs = lba; ; cs = lbas[cs - 1]) {
						sectors.push_back(cs);
						if (lbas[cs - 1] == cs - 1 || lbas[cs - 1] == 0)
							break;
					}

					//Give the sectors back to the LBA
					while (sectors.size() != sect_count_new) {
						lbas[sectors.back() - 1] = head.first_unused_sector;
						head.first_unused_sector = sectors.back();
						sectors.pop_back();
					}

					//Set the new last sector's target LBA to 0
					lbas[sectors.back() - 1] = 0;
				}
			}

			/*
			 * cn_fs::func::internal::remove_file                          {{{2
			 *
			 * Description:
			 *     Does what the name implies. Removes a file by its LBA. Also
			 *     removes all sectors it owns and allows the LBA to write to
			 *     them again.
			 */

			void remove_file(bstream& buf, unsigned int lba) {
				//Get buffer information
				cn_fs::fs_header &head = buf.at<cn_fs::fs_header>(0);
				unsigned int     *lbas = buf.data<unsigned int>() + 6;

				//Grab the stat of this file
				cn_fs::fs_stat *s = &buf.at<cn_fs::fs_stat>(
					head.sect_size * lba
				);

				//If it is a directory then we must go and delete all of those
				//files as well.
				if (s->st_mode == cn_fs::T_DIR) {
					//Go on and load up that directory.
					cn_fs::file fp;
					fp.setup(*cn_fs::global::buf, lba);

					cn_fs::dir directory(fp.bytes);

					//Go through every file and delete it... Recursively
					map<string, unsigned int>::iterator ii;
					for (
						ii = directory.files.begin();
						ii != directory.files.end();
						ii++
					) {
						cn_fs::func::internal::remove_file(
							buf,
							ii->second
						);
					}
				}

				//Cheat. Resize the file to 0 and then change the LBA to be
				//the header's first unallocated sector.
				cn_fs::func::internal::resize_file(buf, lba, 0);

				//Rearrange pointers.
				lbas[lba - 1] = head.first_unused_sector;
				head.first_unused_sector = lba;

				printf("Removed LBA: %d\n", lba);
			}
		}

		// 1}}}

		// --------------------------------------------------------------------
		// 4. Shell Command Implementation                                 {{{1
		// --------------------------------------------------------------------
		
		/*
		 * These are the actual commands that can be run by the user. They are
		 * powered by function pointers that are set up in both shell.cpp and
		 * main.cpp. To add more, simply add more functions and add them to the
		 * command map.
		 */

		/*
		 * cn_fs::func::mkfs                                               {{{2
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
			cn_fs::global::buf =
				new bstream(disk_size, BST_READ | BST_WRITE | BST_ZERO);

			//Format the "disk" to be ready for use.
			cn_fs::fs_header& header =
				cn_fs::global::buf->at<cn_fs::fs_header>(0);

			//Set the first unused sector to sector S.
			header.first_unused_sector = 1;
			header.root_sector = 1;

			//TODO: Create a directory and then dump to that sector.

			//Compute D and S.
			unsigned int N, D, S; //Nintendo DS
			N = ((disk_size / sect_size) + 6) * sizeof(unsigned int);
			S = 1 + ((N - 1) / sect_size);
			D = (N / sizeof(unsigned int)) - 6 - S;

			header.S = S;
			header.D = D;

			header.size      = disk_size;
			header.sect_size = sect_size;

			//Set up the sector pointers... in-place.
			unsigned int* sec_ptr =
				cn_fs::global::buf->data<unsigned int>() + 6;

			//Assign the "next" sector. Last sector has "0"
			for (int i = 0; i < D; i++)
				sec_ptr[i] = (i == D - 1)
					? 0
					: i + 2;

			//...And shuffle it (Fisher-Yates)
			/*
			for (int i = D - 1; i > 0; i--) {
				int j, k;
				j = rand() % (i + 1);

				//Swap
				k = sec_ptr[i];
				sec_ptr[i] = sec_ptr[j];
				sec_ptr[j] = k;
			}
			*/

			//Create the root directory.
			cn_fs::func::internal::inject_file(
				*cn_fs::global::buf,
				1,
				cn_fs::T_DIR
			);

			//Write a "00 00 00 00" to it to tell that there are no files.
			cn_fs::func::internal::resize_file(
				*cn_fs::global::buf,
				1,
				sizeof(uint32_t)
			);

			//Set the current directory to the root directory.
			cn_fs::global::cur_dir = 1;
			cn_fs::global::dir_path.push_back(1);

			//Dump to disk
			vector<string> a;
			cn_fs::func::dump(a);

			//Return success code.
			return 0;
		}

		/*
		 * cn_fs::func::open                                               {{{2
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
		 * cn_fs::func::read                                               {{{2
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
		 * cn_fs::func::write                                              {{{2
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
		 * cn_fs::func::seek                                               {{{2
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
		 * cn_fs::func::close                                              {{{2
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
		 * cn_fs::func::mkdir                                              {{{2
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
			//Grab the header and LBA list
			cn_fs::fs_header &head =
				cn_fs::global::buf->at<cn_fs::fs_header>(0);

			unsigned int *lbas =
				cn_fs::global::buf->data<unsigned int>() + 6;
			
			uint32_t target_sector = head.first_unused_sector;

			/*
			 * CHECK 1 - Available sectors for a new directory
			 */

			if (target_sector == 0) {
				cn_fs::util::log_error(
					"[ERROR][MKDIR] Not enough space on the disk.\n"
				);
				return 1;
			}

			uint32_t next_new_sect = lbas[target_sector - 1];

			//Read from the current directory
			cn_fs::file fp;
			fp.setup(*cn_fs::global::buf, cn_fs::global::cur_dir);

			cn_fs::dir directory(fp.bytes);

			/*
			 * CHECK 2 - If the allocated bytes goes over SECT_SIZE and there's
			 *           no free sectors.
			 */
			
			//Predict the size after writing.
			size_t predicted_size = fp.bytes.size() % head.sect_size;
			predicted_size += sizeof(uint32_t) * 2;
			predicted_size += args[1].size();

			//If the first sector is the only one allocated, consider fs_stat.
			if (fp.bytes.size() < head.sect_size - sizeof(cn_fs::fs_stat))
				predicted_size += sizeof(cn_fs::fs_stat);

			//If we have to use another sector for this directory and there is
			//no more sectors to put the new directory...
			if (
				lbas[target_sector - 1] == 0 &&
				predicted_size > head.sect_size
			) {
				cn_fs::util::log_error(
					"[ERROR][MKDIR] Not enough space on the disk.\n"
				);
				return 1;
			}

			//Inject a directory.
			directory.files.insert(make_pair<string, unsigned int>(
				args[1],
				target_sector
			));

			//Create the root directory.
			cn_fs::func::internal::inject_file(
				*cn_fs::global::buf,
				target_sector,
				cn_fs::T_DIR
			);

			//Write a "00 00 00 00" to it to tell that there are no files.
			cn_fs::func::internal::resize_file(
				*cn_fs::global::buf,
				target_sector,
				sizeof(uint32_t)
			);

			//Update the first unused sector
			head.first_unused_sector = next_new_sect;

			//Commit the changes back
			directory.save();
			fp.save();

			//Dump to disk
			//TODO: Stream this
			vector<string> a;
			cn_fs::func::dump(a);
		}

		/*
		 * cn_fs::func::rmdir                                              {{{2
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
			/*
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"rmdir"
			);
			return 1;
			*/
			if (args.size() < 2) {
				cn_fs::util::log_error(
					"Usage: rmdir dirname [dirname [dirname [...]]]\n"
				);
				return 1;
			}

			cn_fs::fs_header &head =
				cn_fs::global::buf->at<cn_fs::fs_header>(0);

			//Read from the current directory
			cn_fs::file fp;
			fp.setup(*cn_fs::global::buf, cn_fs::global::cur_dir);

			cn_fs::dir directory(fp.bytes);

			cn_fs::fs_stat *s;

			//Check if the directory exists. If it does, remove it.
			map<string, unsigned int>::iterator ii;
			ii = directory.files.find(args[1]);

			if (ii != directory.files.end()) {
				//Jump to the stat struct for the file.
				s = &cn_fs::global::buf->at<cn_fs::fs_stat>(
					head.sect_size * ii->second
				);

				//Check if it is a directory
				if (s->st_mode == cn_fs::T_DIR) {
					//Yup, it's a directory

					//NOTE: This function is recursive. If the directory has
					//any files or directories in it, it will delete them
					//first.
					
					cn_fs::func::internal::remove_file(
						*cn_fs::global::buf,
						ii->second
					);

					//Erase from the map
					directory.files.erase(ii);
				}
				else {
					//Nope. Not a directory. It's a file.
					cn_fs::util::log_error(
						"%s: failed to remove '%s': %s\n",
						"rmdir",
						args[1].c_str(),
						"Not a directory"
					);
					return 1;
				}
			}
			else {
				cn_fs::util::log_error(
					"%s: failed to remove '%s': %s\n",
					"rmdir",
					args[1].c_str(),
					"No such file or directory"
				);
				return 1;
			}

			//Commit the changes back
			directory.save();
			fp.save();

			//Dump to disk
			//TODO: Stream this
			vector<string> a;
			cn_fs::func::dump(a);
		}

		/*
		 * cn_fs::func::cd                                                 {{{2
		 * 
		 * Description:
		 *     Change the current directory to "dirname".
		 *
		 * Example:
		 *     cd ../../foo/bar
		 *
		 * CN_FS Syntax:
		 *     cd dirname
		 */

		int cd(_ARGS& args) {
			/*
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"cd"
			);
			return 1;
			*/

			//Argument Check
			if (args.size() != 2) {
				cn_fs::util::log_error(
					"Usage: cd dirname\n"
				);
				return 1;
			}

			cn_fs::fs_header &head =
				cn_fs::global::buf->at<cn_fs::fs_header>(0);

			//Create a "savestate" that we can go back to if the command fails
			deque<uint32_t> state_path = cn_fs::global::dir_path;
			uint32_t state_block = cn_fs::global::cur_dir;

			//Explode the path into separate parts
			vector<string> paths;
			cn_fs::util::explode(paths, args[1].c_str(), "/");

			for (int i = 0; i < paths.size(); i++) {
				printf("[DEBUG][CD] %s\n", paths[i].c_str());
			}

			for (int i = 0; i < paths.size(); i++) {
				if (paths[i] == "..") {
					//Go back a directory
					cn_fs::global::dir_path.pop_back();

					//Make it impossible to go past the root directory...
					if (cn_fs::global::dir_path.size() == 0)
						cn_fs::global::dir_path.push_back(head.root_sector);

					cn_fs::global::cur_dir = cn_fs::global::dir_path.back();
				}
				else
				if (paths[i] == "." || paths[i] == "") {
					//...It's the same directory.
					continue;
				}
				else {
					//Read from the current directory
					cn_fs::file fp;
					fp.setup(*cn_fs::global::buf, cn_fs::global::cur_dir);
					cn_fs::dir directory(fp.bytes);

					//Setup
					map<string, unsigned int>::iterator ii;

					//Find the directory and act on whether it exists or not
					ii = directory.files.find(paths[i]);

					if (ii == directory.files.end()) {
						//We failed to find the file. Print an error.
						cn_fs::util::log_error(
							"[ERROR][CD] No such file or directory: %s\n",
							args[1].c_str()
						);

						//Revert
						cn_fs::global::dir_path = state_path;
						cn_fs::global::cur_dir  = state_block;

						//Return an error
						return 1;
					}

					//Assume it is found and is in "ii"
					cn_fs::global::cur_dir = ii->second;
					cn_fs::global::dir_path.push_back(ii->second);
				}
			}

			return 0;
		}

		/*
		 * cn_fs::func::ls                                                 {{{2
		 * 
		 * Description:
		 *     Show the content of the current directory. No parameters need to
		 *     be supported.
		 *
		 * CN_FS Syntax:
		 *     ls
		 */

		int ls(_ARGS& args) {
			/*
			cn_fs::util::log_error(
				"[ERROR] %s has not been implemented yet.\n",
				"ls"
			);
			return 1;
			*/
			
			//Read from the current directory
			cn_fs::file fp;
			fp.setup(*cn_fs::global::buf, cn_fs::global::cur_dir);
			cn_fs::dir directory(fp.bytes);

			//Setup
			map<string, unsigned int>::iterator ii;

			//Just iterate through the directory's map and print out
			for (
				ii  = directory.files.begin();
				ii != directory.files.end();
				ii++
			) {
				printf("%s\n", ii->first.c_str());
			}
		}

		/*
		 * cn_fs::func::cat                                                {{{2
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
		 * cn_fs::func::tree                                               {{{2
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
		 * cn_fs::func::import                                             {{{2
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
		 * cn_fs::func::_export                                            {{{2
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

		/*
		 * cn_fs::func::dump
		 *
		 * Description:
		 *     Dumps the entire file system to disk. Use for debugging purposes
		 *     only.
		 *
		 * Example:
		 *     dump
		 *
		 * CN_FS Syntax:
		 *     dump
		 */

		int dump(_ARGS& args) {
			//Get header information.
			cn_fs::fs_header &head =
				cn_fs::global::buf->at<cn_fs::fs_header>(0);

			//Dump all bytes to disk.
			FILE *fp;
			fp = fopen("disk.cndisk", "wb");
			fwrite(
				&cn_fs::global::buf->data()[0],
				sizeof(char),
				head.size,
				fp
			);
			fclose(fp);
		}

		// 1}}}
	}

	// ------------------------------------------------------------------------
	// 5. Global Variables (cn_fs::global)                                 {{{1
	// ------------------------------------------------------------------------
	namespace global {
		uint32_t cur_dir = 0;
		deque<uint32_t> dir_path;
		bstream* buf = NULL;
		map<size_t, cn_fs::file> fd_dir;
	}
}
