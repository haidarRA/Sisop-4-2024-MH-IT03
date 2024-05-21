
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

static const char *relics_path = "/home/soal_3/relics";

static int relicfs_getattr(const char *path, struct stat *stbuf) {
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else {
        char fpath[1024];
        snprintf(fpath, sizeof(fpath), "%s%s.000", relics_path, path);
        res = stat(fpath, stbuf);
        if (res == -1) return -errno;

        // Calculate combined file size
        off_t total_size = 0;
        int part = 0;
        while (1) {
            snprintf(fpath, sizeof(fpath), "%s%s.%03d", relics_path, path, part);
            struct stat part_stat;
            if (stat(fpath, &part_stat) == -1) break;
            total_size += part_stat.st_size;
            part++;
        }
        stbuf->st_size = total_size;
    }
    return res;
}

static int relicfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0) return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    DIR *dp;
    struct dirent *de;
    dp = opendir(relics_path);
    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        if (strstr(de->d_name, ".000")) {
            char name[1024];
            strncpy(name, de->d_name, strlen(de->d_name) - 4);
            name[strlen(de->d_name) - 4] = '\0';
            filler(buf, name, NULL, 0);
        }
    }
    closedir(dp);
    return 0;
}

static int relicfs_open(const char *path, struct fuse_file_info *fi) {
    return 0;
}

static int relicfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;

    char fpath[1024];
    snprintf(fpath, sizeof(fpath), "%s%s", relics_path, path);

    size_t total_size = 0;
    size_t read_size = 0;
    int part = 0;
    while (1) {
        snprintf(fpath, sizeof(fpath), "%s%s.%03d", relics_path, path, part);
        int fd = open(fpath, O_RDONLY);
        if (fd == -1) break;

        struct stat st;
        stat(fpath, &st);
        size_t part_size = st.st_size;
        char *part_buf = (char *)malloc(part_size);
        pread(fd, part_buf, part_size, 0);
        close(fd);

        if (offset < total_size + part_size) {
            size_t part_offset = offset - total_size;
            read_size = size < (part_size - part_offset) ? size : (part_size - part_offset);
            memcpy(buf, part_buf + part_offset, read_size);
            free(part_buf);
            return read_size;
        }
        total_size += part_size;
        free(part_buf);
        part++;
    }
    return 0;
}

static int relicfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    char fpath[1024];
    snprintf(fpath, sizeof(fpath), "%s%s.000", relics_path, path);
    int fd = open(fpath, O_CREAT | O_WRONLY, mode);
    if (fd == -1) return -errno;
    close(fd);
    return 0;
}

static int relicfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;

    char fpath[1024];
    snprintf(fpath, sizeof(fpath), "%s%s", relics_path, path);

    size_t total_size = 0;
    int part = 0;
    while (1) {
        snprintf(fpath, sizeof(fpath), "%s%s.%03d", relics_path, path, part);
        struct stat st;
        if (stat(fpath, &st) == -1) break;

        size_t part_size = st.st_size;
        if (offset < total_size + part_size) {
            size_t part_offset = offset - total_size;
            size_t write_size = size < (part_size - part_offset) ? size : (part_size - part_offset);
            int fd = open(fpath, O_WRONLY);
            pwrite(fd, buf, write_size, part_offset);
            close(fd);
            return write_size;
        }
        total_size += part_size;
        part++;
    }

    // Create new part if necessary
    snprintf(fpath, sizeof(fpath), "%s%s.%03d", relics_path, path, part);
    int fd = open(fpath, O_CREAT | O_WRONLY, 0644);
    write(fd, buf, size);
    close(fd);
    return size;
}

static int relicfs_unlink(const char *path) {
    char fpath[1024];
    int part = 0;
    while (1) {
        snprintf(fpath, sizeof(fpath), "%s%s.%03d", relics_path, path, part);
        if (unlink(fpath) == -1) break;
        part++;
    }
    return 0;
}

static struct fuse_operations relicfs_oper = {
    .getattr  = relicfs_getattr,
    .readdir  = relicfs_readdir,
    .open     = relicfs_open,
    .read     = relicfs_read,
    .create   = relicfs_create,
    .write    = relicfs_write,
    .unlink   = relicfs_unlink,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &relicfs_oper, NULL);
}