CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC = src/nfs_server.c src/file_functions.c src/cmd_parser.c
OUT = bin/nfs_server

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)