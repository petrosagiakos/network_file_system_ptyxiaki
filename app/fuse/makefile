CC = gcc

CFLAGS = -Wall -Wextra -D_FILE_OFFSET_BITS=64

FUSE_CFLAGS = $(shell pkg-config fuse --cflags)
FUSE_LIBS   = $(shell pkg-config fuse --libs)

TARGET = testfs

SRC = test_fs.c protocol_text.c

all: $(TARGET)

$(TARGET): $(SRC) protocol_text.h
	$(CC) $(CFLAGS) $(FUSE_CFLAGS) $(SRC) $(FUSE_LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean