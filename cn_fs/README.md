# CN\_FS
*An attempt at a file system that can also be a package file*

## Synopsis


## Function Implementation Progress

### Assignment Stated
```diff
+ mkfs
- open
- read
- write
- seek
- close
+ mkdir
+ rmdir
+ cd
+ ls
- cat
- tree
- import
- export
```

### Extra Functions
```diff
- lsof
- stat
- pwd
+ dump
```

## How it works
### In general
The idea behind this file system is extremely similar to Dr. Plank's FATRW assignment (cs594\_jp). Sectors are split up as S and D, which is computed on format time. The following struct is used to determine data in the entire disk:

```c
struct fs_header {
	unsigned int first_unused_sector;
	unsigned int root_sector;
	unsigned int S, D;
};
```

### Files and Directories
Every directory and file is treated the exact same... as a file. However, the file system will be able to differentiate the two, as certain flags will be set in a stat struct to determine it. Directories are simply files that have entries in them to point to other sectors. Whether those sectors are files or not is dependent on the stat struct for them. Directories and files share the same stat struct:

```cpp
struct cn_stat {
	mode_t st_mode;          /* Tell whether it's a directory or a file */
	off_t st_size;           /* Size of file */
	struct timespec st_atim; /* Time of last access */
	struct timespec st_mtim; /* Time of last modification */
	struct timespec st_ctim; /* Time of last status change */
};
```

Since the file system is broken up into blocks. Files must know where the next sector in the disk is located. This is done via the first "S" sectors, where it will explicitly point to which sector goes next.

### Directory Structure
Directories are treated as files, as they should. The format of a directory is as follows:
```
4 bytes - number of files

Repeat N times (where N is the number of files)
	4 bytes - sector num
	4 bytes - file name length
	char[] - file name
```
