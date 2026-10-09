#define FUSE_USE_VERSION 26 // Αλλαγή έκδοσης για FUSE 2 compatibility

#include <fuse.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>

#include "protocol_text.h"


FILE *logfile = NULL;

static int sock_fd = -1;


static int sp_open(const char *path, struct fuse_file_info *fi)
{
    int fd;
    fprintf(logfile, "sp_open for %s\n", path);
    fd = server_open(sock_fd,path, fi->flags);
    if (fd == -1) {
        return -errno;
    }
    fprintf(logfile, "sp_open got fd = %d\n", fd);
    fi->fh = fd;
    return 0;
}

static int sp_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    size_t sz;
    int fd;
    if (fi == NULL) {
        fprintf(logfile, "sp_read for %s (open first)\n", path);
        fd = server_open(sock_fd,path, O_RDONLY);
    }
    else {
        fd = fi->fh;
        fprintf(logfile, "sp_read for %s (got fd = %d)\n", path, fd);
    }
    if (fd == -1) {
        return -errno;
    }
    sz = server_read(sock_fd,fd, buf, size, offset);
    if (sz == -1) {
        return -errno;
    }
    if (fi == NULL) {
        close(fd);
    }
    return sz;
}

static int sp_getattr(const char *path, struct stat *stbuf)
{
    int error;
    fprintf(logfile, "sp_getattr for %s\n", path);
    error = lstat(path, stbuf);
    if (error == -1) {
        error = -errno;
    }
    return error;
}

static int sp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
           off_t offset, struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;
    
    fprintf(logfile, "sp_readdir for %s\n", path);
    dp = opendir(path);
    if (dp == NULL) {
        return -errno;
    }
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        
        // Στο FUSE 2, το filler δέχεται 4 ορίσματα και επιστρέφει 1 σε αποτυχία
        if (filler(buf, de->d_name, &st, 0) != 0) {
            break;
        }
    }
    closedir(dp);
    return 0;
}

static struct fuse_operations spfs_operations = {
    .open = sp_open,
    .read = sp_read,
    .getattr = sp_getattr,
    .readdir = sp_readdir,
};

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr,
                "Usage: %s <mountpoint> <server_ip>\n",
                argv[0]);

        return EXIT_FAILURE;
    }
    logfile = fopen("spfs.log", "w+");
    
    if (logfile == NULL) {
        perror("spfs.log");
        return EXIT_FAILURE;
    }

    setbuf(logfile, NULL);

    fprintf(logfile,
            "Connecting to server %s:%s\n",
            argv[2],
            PORT);

    sock_fd = init_client(argv[2]);


    if (sock_fd < 0) {
        fprintf(stderr,
                "Failed to connect to server %s:%s\n",
                argv[2],
                PORT);

        fclose(logfile);

        return EXIT_FAILURE;
    }

    fprintf(logfile,
            "Connected. socket fd=%d\n",
            sock_fd);
    
    char *fuse_argv[2];

    fuse_argv[0] = argv[0];
    fuse_argv[1] = argv[1];

    int fuse_argc = 2;

    int result =
        fuse_main(fuse_argc,
                  fuse_argv,
                  &spfs_operations,
                  NULL);
    close_sock(sock_fd);
    sock_fd = -1;

    fclose(logfile);

    return result;
    
}