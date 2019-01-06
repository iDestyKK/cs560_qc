/*
 * CN_FS: Package Format that can function as a disk / Entry File
 *
 * Description:
 *     CN_FS is a File System that can operate as both a package/container and
 *     a live file system. This is implemented as a proof-of-concept design for
 *     an assignment. But realistically it can be used in combination with GZ,
 *     BZ2, or XZ to make an efficient package file in the same fashion as TAR.
 *
 * Author:
 *     Clara Nguyen
 */

//C++ Includes
#include <iostream>
#include <string>
#include <vector>

//Utility files
#include "cn_fs.hpp"
#include "incl/shell.hpp"

using namespace std;
using namespace handy;

int main() {
	//Set up the shell
	cn_fs::shell sh;
	string cmd;

	//Set up the commands for the shell
	sh.command_add("mkfs"  , cn_fs::func::mkfs   );
	sh.command_add("open"  , cn_fs::func::open   );
	sh.command_add("read"  , cn_fs::func::read   );
	sh.command_add("write" , cn_fs::func::write  );
	sh.command_add("seek"  , cn_fs::func::seek   );
	sh.command_add("close" , cn_fs::func::close  );
	sh.command_add("mkdir" , cn_fs::func::mkdir  );
	sh.command_add("rmdir" , cn_fs::func::rmdir  );
	sh.command_add("cd"    , cn_fs::func::cd     );
	sh.command_add("ls"    , cn_fs::func::ls     );
	sh.command_add("cat"   , cn_fs::func::cat    );
	sh.command_add("tree"  , cn_fs::func::tree   );
	sh.command_add("import", cn_fs::func::import );
	sh.command_add("export", cn_fs::func::_export);

	//Run the shell and interpreter. Exit when done.
	return sh.run();
}
