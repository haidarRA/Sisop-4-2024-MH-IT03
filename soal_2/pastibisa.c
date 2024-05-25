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
#define SECRET_PASSWORD "sisopgampang"

/*
Compile:
gcc -Wall `pkg-config fuse --cflags` pastibisa.c -o pastibisa `pkg-config fuse --libs` -lssl -lcrypto
Note: pastikan openssl sudah terinstall dengan menggunakan command 'sudo apt install libssl-dev'
*/

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

void decode_base64(const char *filename) {
    FILE *file = fopen(filename, "r+");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(file_length + 1);
    if (!content) {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    fread(content, 1, file_length, file);
    content[file_length] = '\0';

    BIO *bio, *b64;
    char *decoded = (char *)malloc(file_length);
    if (!decoded) {
        perror("Failed to allocate memory");
        fclose(file);
        free(content);
        return;
    }

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(content, -1);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int decoded_length = BIO_read(bio, decoded, file_length);
    BIO_free_all(bio);

    fclose(file);

    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        free(content);
        free(decoded);
        return;
    }

    fwrite(decoded, 1, decoded_length, file);

    free(content);
    free(decoded);
    fclose(file);
}

void decode_rot13(const char *filename) {
    FILE *file = fopen(filename, "r+");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(file_length + 1);
    if (!content) {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    fread(content, 1, file_length, file);
    content[file_length] = '\0';

    for (long i = 0; i < file_length; i++) {
        if ('a' <= content[i] && content[i] <= 'z') {
            content[i] = 'a' + (content[i] - 'a' + 13) % 26;
        } else if ('A' <= content[i] && content[i] <= 'Z') {
            content[i] = 'A' + (content[i] - 'A' + 13) % 26;
        }
    }

    fclose(file);

    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        free(content);
        return;
    }

    fwrite(content, 1, file_length, file);

    free(content);
    fclose(file);
}

void decode_hex(const char *filename) {
    FILE *file = fopen(filename, "r+");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(file_length + 1);
    if (!content) {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    fread(content, 1, file_length, file);
    content[file_length] = '\0';

    char *decoded = (char *)malloc(file_length / 2 + 1);
    if (!decoded) {
        perror("Failed to allocate memory");
        fclose(file);
        free(content);
        return;
    }

    for (long i = 0; i < file_length; i += 2) {
        sscanf(content + i, "%2hhx", &decoded[i / 2]);
    }

    fclose(file);

    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        free(content);
        free(decoded);
        return;
    }

    fwrite(decoded, 1, file_length / 2, file);

    free(content);
    free(decoded);
    fclose(file);
}

void reverse(const char *filename) {
    FILE *file = fopen(filename, "r+");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(file_length + 1);
    if (!content) {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    fread(content, 1, file_length, file);
    content[file_length] = '\0'; // Null-terminate the content

    fclose(file);

    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        free(content);
        return;
    }

    long start = 0;
    for (long end = 0; end <= file_length; ++end) {
        if (content[end] == '\n' || content[end] == '\0') {
            for (long i = end - 1; i >= start; --i) {
                fputc(content[i], file);
            }
            if (content[end] == '\n') {
                fputc('\n', file);
            }
            start = end + 1;
        }
    }

    free(content);
    fclose(file);
}

//static const char *root_directory = "/home/ubuntu/soal_2/sensitif";
static const char *root_directory = "/home/haidar/soal_2/sensitif";
static int access_allowed = 0;
static const char *secret_directory = "/rahasia-berkas";

int check_password() {
    char input[256];
    printf("Enter password to access %s: ", secret_directory);
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }
    input[strcspn(input, "\n")] = 0;
    return strcmp(input, SECRET_PASSWORD) == 0;
}

static int check_access(const char *path) {
    if (strncmp(path, secret_directory, strlen(secret_directory)) == 0) {
        if (!access_allowed && !check_password()) {
            return -EACCES;
        }
        access_allowed = 1;

    }
    return 0;
}

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    int result, resacc;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", root_directory, path);

    if((strstr(full_path, "/rahasia-berkas") && !strstr(full_path, "/rahasia-berkas/")) && !is_regular_file(full_path)) {
	resacc = check_access(path);
	if (resacc != 0)  {
	    write_log("FAILED", "accessDenied", "Incorrect password for 'rahasia-berkas' folder");
	    return resacc;
	}
	else if (resacc == 0) {
	    write_log("SUCCESS", "accessGranted", "Access granted to 'rahasia-berkas' folder");
	}
    }

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
    int result, resacc;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", root_directory, path);
    
    result = open(full_path, fi->flags);
    if (result == -1) return -errno;

    fi->fh = result;
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int result, resacc;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", root_directory, path);

    if (strstr(full_path, "rev")) {
        reverse(full_path);
        write_log("SUCCESS", "decodeRev", "File decoded successfully using reverse");
    } 
    else if (strstr(full_path, "base64")) {
        decode_base64(full_path);
        write_log("SUCCESS", "decodeBase64", "File decoded successfully using base64");
    } 
    else if (strstr(full_path, "rot13")) {
        decode_rot13(full_path);
        write_log("SUCCESS", "decodeRot13", "File decoded successfully using rot13");
    } 
    else if (strstr(full_path, "hex")) {
        decode_hex(full_path);
        write_log("SUCCESS", "decodeHex", "File decoded successfully using hex");
    }

    int fd = open(full_path, O_RDONLY);
    if (fd == -1) return -errno;

    result = pread(fd, buf, size, offset);
    if (result == -1) result = -errno;

    close(fd);

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

