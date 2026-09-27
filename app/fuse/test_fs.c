#define FUSE_USE_VERSION 26 // Αλλαγή έκδοσης για FUSE 2 compatibility

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <unistd.h>    // Για pread και close
#include <sys/types.h> // Για lstat και καταλόγους
#include <sys/stat.h>  // Για struct stat
#include <dirent.h>    // Για DIR, opendir, readdir, struct dirent

// Δήλωση της καθολικής μεταβλητής για το log file
FILE *logfile = NULL;

static int
sp_open(const char *path, struct fuse_file_info *fi)
{
    int fd;
    fprintf(logfile, "sp_open for %s\n", path);
    fd = open(path, fi->flags);
    if (fd == -1) {
        return -errno;
    }
    fprintf(logfile, "sp_open got fd = %d\n", fd);
    fi->fh = fd;
    return 0;
}

static int
sp_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    size_t sz;
    int fd;
    if (fi == NULL) {
        fprintf(logfile, "sp_read for %s (open first)\n", path);
        fd = open(path, O_RDONLY);
    }
    else {
        fd = fi->fh;
        fprintf(logfile, "sp_read for %s (got fd = %d)\n", path, fd);
    }
    if (fd == -1) {
        return -errno;
    }
    sz = pread(fd, buf, size, offset);
    if (sz == -1) {
        return -errno;
    }
    if (fi == NULL) {
        close(fd);
    }
    return sz;
}

static int
sp_getattr(const char *path, struct stat *stbuf)
{
    int error;
    fprintf(logfile, "sp_getattr for %s\n", path);
    error = lstat(path, stbuf);
    if (error == -1) {
        error = -errno;
    }
    return error;
}

static int
sp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
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

int
main(int argc, char *argv[])
{
    logfile = fopen("spfs.log", "w+");
    if (logfile == NULL) {
        printf("spfs: failed to open logfile\n");
        return -1;
    } else {
        setbuf(logfile, NULL);
        return fuse_main(argc, argv, &spfs_operations, NULL);
    }
}