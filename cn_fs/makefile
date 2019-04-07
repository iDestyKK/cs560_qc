# CN_FS demo makefile
CC      = g++
CFLAGS  = --std=c++98 -g
EXEC    = cn_fs
HANDY_D = lib/handy
INCL_D  = incl

all: $(EXEC)

cn_fs: main.cpp cn_fs.cpp $(HANDY_D)/bstream.cpp $(HANDY_D)/file.cpp $(INCL_D)/shell.cpp $(INCL_D)/utility.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(EXEC)
