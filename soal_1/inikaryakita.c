#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <zlib.h>
#include <zip.h>
#include <stdlib.h>
#include <libgen.h>
#include <wand/MagickWand.h>

static const char *dirpath = "/home/haidar/sisop/modul4/soal_1";

/*
Compile:
gcc -Wall `pkg-config fuse --cflags` inikaryakita.c -o inikaryakita `pkg-config fuse --libs` `pkg-config --cflags --libs MagickWand` -lcurl -lzip
*/

/*
Test reverse:
nakaynatid hasus ualak ,ulud aja abociD
*/

int directory_exists(const char *path) {
    struct stat stats;
    if (stat(path, &stats) != 0) {
        return 0;
    }

    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}

int download_file() {
    CURL *curl;
    CURLcode res;
    FILE *fp;
    const char *url = "https://drive.google.com/uc?export=download&id=1VP6o84AQfY6QbghFGkw6ghxkBbv7fpum";
    const char *filename = "portofolio.zip";

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(filename, "wb");
        if (fp) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            // Follow redirects
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                fclose(fp);
                curl_easy_cleanup(curl);
                return 1;
            }

            fclose(fp);
            curl_easy_cleanup(curl);
            return 0;
        } else {
            fprintf(stderr, "Error opening file %s\n", filename);
            curl_easy_cleanup(curl);
            return 1;
        }
    } else {
        fprintf(stderr, "curl_easy_init() failed\n");
        return 1;
    }
}

void add_watermark(const char *input_image, const char *watermark_text, const char *output_image) {
    MagickWand *wand;
    DrawingWand *drawing_wand;
    PixelWand *text_wand;

    MagickWandGenesis();

    wand = NewMagickWand();
    if (wand == NULL) {
        fprintf(stderr, "Failed to create MagickWand\n");
        return;
    }

    if (MagickReadImage(wand, input_image) == MagickFalse) {
        fprintf(stderr, "Failed to read image: %s\n", input_image);
        DestroyMagickWand(wand);
        return;
    }

    drawing_wand = NewDrawingWand();
    if (drawing_wand == NULL) {
        fprintf(stderr, "Failed to create DrawingWand\n");
        DestroyMagickWand(wand);
        return;
    }

    text_wand = NewPixelWand();
    if (text_wand == NULL) {
        fprintf(stderr, "Failed to create PixelWand\n");
        DestroyDrawingWand(drawing_wand);
        DestroyMagickWand(wand);
        return;
    }
    PixelSetColor(text_wand, "white");

    DrawSetFont(drawing_wand, "Arial");
    DrawSetFontSize(drawing_wand, 36);  // Adjust font size as desired
    DrawSetFillColor(drawing_wand, text_wand);
    DrawSetGravity(drawing_wand, SouthGravity);

    MagickAnnotateImage(wand, drawing_wand, 0, 0, 0, watermark_text);

    if (MagickWriteImages(wand, output_image, MagickTrue) == MagickFalse) {
        fprintf(stderr, "Failed to write image: %s\n", output_image);
    }

    DestroyPixelWand(text_wand);
    DestroyDrawingWand(drawing_wand);
    DestroyMagickWand(wand);

    MagickWandTerminus();
}

void make_parent_dirs(const char *path) {
    char *dup_path = strdup(path);
    char *dir = dirname(dup_path);
    char temp_path[1024] = {0};
    char *p = NULL;

    for (p = strtok(dir, "/"); p != NULL; p = strtok(NULL, "/")) {
        strcat(temp_path, "/");
        strcat(temp_path, p);
        mkdir(temp_path, 0755);
    }
    free(dup_path);
}

int extract_zip(const char *zip_filename, const char *destination) {
    int err = 0;
    struct zip *za = zip_open(zip_filename, 0, &err);
    if (za == NULL) {
        fprintf(stderr, "Error opening zip file: %d\n", err);
        return 1;
    }

    for (int i = 0; i < zip_get_num_entries(za, 0); i++) {
        struct zip_stat st;
        zip_stat_init(&st);
        if (zip_stat_index(za, i, 0, &st) == 0) {
            char *contents = malloc(st.size + 1); // Allocate one extra byte for the null terminator
            if (contents == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                zip_close(za);
                return 1;
            }

            struct zip_file *zf = zip_fopen_index(za, i, 0);
            if (zf) {
                zip_fread(zf, contents, st.size);
                contents[st.size] = '\0'; // Null-terminate the buffer
                zip_fclose(zf);

                char outpath[1024];
                snprintf(outpath, sizeof(outpath), "%s/%s", destination, st.name);

                if (st.name[strlen(st.name) - 1] == '/') {
                    mkdir(outpath, 0755);
                } else {
                    make_parent_dirs(outpath);

                    FILE *outfile = fopen(outpath, "wb");
                    if (outfile) {
                        fwrite(contents, 1, st.size, outfile);
                        fclose(outfile);
                    } else {
                        fprintf(stderr, "Error creating file %s\n", outpath);
                    }
                }
            }
            free(contents);
        }
    }
    zip_close(za);
    return 0;
}

void reverse_file_contents(const char *filename) {
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



static int xmp_unlink(const char *path) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (unlink(fpath) == -1) {
        return -errno;
    }
    return 0;
}

static int xmp_rmdir(const char *path) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (rmdir(fpath) == -1) {
        return -errno;
    }
    return 0;
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];

    sprintf(fpath, "%s%s", dirpath, path);

    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (mkdir(fpath, mode) == -1) {
        return -errno;
    }

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];

    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    } else {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    int res = 0;
    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(fpath);

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        res = (filler(buf, de->d_name, &st, 0));
        if (res != 0)
            break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    int fd;
    int res;

    sprintf(fpath, "%s%s", dirpath, path);

    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;
/*
    if (strstr(fpath, "/test")) {
        reverse_file_contents(fpath);
    }
*/
    close(fd);
    return res;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    int fd;
    int res;

    sprintf(fpath, "%s%s", dirpath, path);

    if (fi == NULL || (fd = fi->fh) == 0) {
        fd = open(fpath, O_WRONLY);
        if (fd == -1)
            return -errno;
    }

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    if (strstr(fpath, "/test")) {
        reverse_file_contents(fpath);
    }

    if (fi == NULL)
        close(fd);

    return res;
}

static int xmp_truncate(const char *path, off_t offset) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (truncate(fpath, offset) == -1) {
        return -errno;
    }

    if (strstr(fpath, "/test")) {
        reverse_file_contents(fpath);
    }

    return 0;
}

static int xmp_rename(const char *from, const char *to) {
    char fpath_from[1000];
    char fpath_to[1000];
    sprintf(fpath_from, "%s%s", dirpath, from);
    sprintf(fpath_to, "%s%s", dirpath, to);

    if (rename(fpath_from, fpath_to) == -1) {
        return -errno;
    }

    if (strstr(fpath_to, "/wm")) {
        add_watermark(fpath_to, "inikaryakita.id", fpath_to);
    }

    return 0;
}

static int xmp_chmod(const char *path, mode_t mode) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (strstr(fpath, "/script.sh")) {
        if (chmod(fpath, 0755) != -1) {
            return 0;
        }
    }

    if (chmod(fpath, mode) == -1) {
        return -errno;
    }

    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    int res = open(fpath, fi->flags);

    if (res == -1)
        return -errno;

    fi->fh = res;
    return 0;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    int res = open(fpath, fi->flags, mode);

    if (res == -1)
        return -errno;

    fi->fh = res;
    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .mkdir = xmp_mkdir,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .write = xmp_write,
    .truncate = xmp_truncate,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .chmod = xmp_chmod,
    .open = xmp_open,
    .create = xmp_create,
};

int main(int argc, char *argv[]) {
    const char *filename = "portofolio";

    if (!directory_exists(filename)) {
        if (download_file() != 0) {
            fprintf(stderr, "Error downloading file\n");
            return 1;
        }
        if (extract_zip("portofolio.zip", ".") != 0) {
            fprintf(stderr, "Error extracting zip file\n");
            return 1;
        }
        sleep(2);
        remove("portofolio.zip");

        if (mkdir("portofolio/gallery/wm", 0777) != 0) {
            fprintf(stderr, "Error creating directory\n");
            return 1;
        }
    }

    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
