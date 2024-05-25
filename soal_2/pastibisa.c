#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

// Pengaturan logging
#define LOG_FILE "fuse_log.txt"

void write_log(const char *status, const char *label, const char *detail) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (!log_file) return;

    time_t current_time;
    time(&current_time);
    struct tm *time_info = localtime(&current_time);

    fprintf(log_file, "[%s]::%02d/%02d/%04d-%02d:%02d:%02d::[%s]::[%s]\n", 
            status, time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900, 
            time_info->tm_hour, time_info->tm_min, time_info->tm_sec, label, detail);
    fclose(log_file);
}

static const char *root_directory = "/home/ubuntu/soal_2/sensitif";
static int access_allowed = 0;

static int verify_access(const char *path) {
    if (strstr(path, "secret-files") != NULL && !access_allowed) {
        char password[256];
        printf("Enter password to access 'secret-files': ");
        scanf("%255s", password);
        if (strcmp(password, "your_password") == 0) {  // Ganti dengan pemeriksaan kata sandi yang sesuai
            access_allowed = 1;
            write_log("SUCCESS", "accessGranted", "Access granted to 'secret-files'");
        } else {
            write_log("FAILED", "accessDenied", "Incorrect password");
            return -EACCES;
        }
    }
    return 0;
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    int result;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", root_directory, path);

    result = lstat(full_path, stbuf);
    if (result == -1) return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dirp;
    struct dirent *dir_entry;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", root_directory, path);

    dirp = opendir(full_path);
    if (!dirp) return -errno;

    while ((dir_entry = readdir(dirp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = dir_entry->d_ino;
        st.st_mode = dir_entry->d_type << 12;
        if (filler(buf, dir_entry->d_name, &st, 0)) break;
    }

    closedir(dirp);
    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    int result;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", root_directory, path);

    result = open(full_path, fi->flags);
    if (result == -1) return -errno;

    fi->fh = result;
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int result;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", root_directory, path);

    int access_result = verify_access(full_path);
    if (access_result != 0) return access_result;

    int fd = fi->fh;
    (void) fd;

    result = pread(fd, buf, size, offset);
    if (result == -1) result = -errno;

    // Dekode konten berdasarkan awalan
    if (strstr(path, "base64_") != NULL || strstr(path, ".base64") != NULL) {
        BIO *bio, *b64;
        BUF_MEM *buffer_ptr;
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new_mem_buf(buf, result);
        bio = BIO_push(b64, bio);
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
        buffer_ptr = BUF_MEM_new();
        BUF_MEM_grow(buffer_ptr, result);
        int decode_length = BIO_read(bio, buffer_ptr->data, result);
        buffer_ptr->length = decode_length;
        buffer_ptr->data[decode_length] = '\0';
        BIO_free_all(bio);
        strncpy(buf, buffer_ptr->data, decode_length);
        BUF_MEM_free(buffer_ptr);
        result = decode_length;
    } else if (strstr(path, "rot13_") != NULL || strstr(path, ".rot13") != NULL) {
        for (int i = 0; i < result; i++) {
            if ((buf[i] >= 'a' && buf[i] <= 'z')) {
                buf[i] = (buf[i] - 'a' + 13) % 26 + 'a';
            } else if ((buf[i] >= 'A' && buf[i] <= 'Z')) {
                buf[i] = (buf[i] - 'A' + 13) % 26 + 'A';
            }
        }
    } else if (strstr(path, "hex_") != NULL || strstr(path, ".hex") != NULL) {
        char decoded_buffer[result / 2];
        for (int i = 0; i < result; i += 2) {
            sscanf(&buf[i], "%2hhx", &decoded_buffer[i / 2]);
        }
        memcpy(buf, decoded_buffer, result / 2);
        result /= 2;
    } else if (strstr(path, "rev_") != NULL || strstr(path, ".rev") != NULL) {
        for (int i = 0; i < result / 2; i++) {
            char temp = buf[i];
            buf[i] = buf[result - i - 1];
            buf[result - i - 1] = temp;
        }
    }

    return result;
}

static struct fuse_operations xmp_operations = {
    .getattr    = xmp_getattr,
    .readdir    = xmp_readdir,
    .open       = xmp_open,
    .read       = xmp_read,
};

int main(int argc, char *argv[]) { 
    umask(0);
    return fuse_main(argc, argv, &xmp_operations, NULL);
}

