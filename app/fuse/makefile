all:
	gcc -Wall test_fs.c -D_FILE_OFFSET_BITS=64 $(shell pkg-config fuse --cflags --libs) -o testfs