#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int is_line(const char *line, int size) {
    for (int i = 0; i < size; ++i) {
        if (line[i] == '\n') {
            return i + 1;
        }
    }
    return -1;
}

char *find_line_lib(FILE *fp) {

    size_t tmp_size = 256;
    char *tmp_string = (char *) calloc(tmp_size, sizeof(char));
    size_t readed = fread(tmp_string, sizeof(char), tmp_size, fp);
    int have_line = is_line(tmp_string, readed);

    while (have_line == -1 && readed == tmp_size) {
        fseek(fp, -readed, SEEK_CUR);
        tmp_size *= 2;

        free(tmp_string);
        tmp_string = (char *) calloc(tmp_size, sizeof(char));
        readed = fread(tmp_string, sizeof(char), tmp_size, fp);
        have_line = is_line(tmp_string, readed);
    }

    char *result;
    if (have_line == -1) { //end of file
        result = (char *) calloc(readed + 2, sizeof(char));
        strncpy(result, tmp_string, readed);
        result[readed] = '\0';
    } else {
        result = (char *) calloc(have_line + 2, sizeof(char));
        strncpy(result, tmp_string, have_line);
        fseek(fp, -(readed - have_line), SEEK_CUR); // <-- 2 godziny rozpaczy
        result[have_line] = '\0';
    }
    free(tmp_string);
    return result;
}

int is_non_backward(const char *line) {
    if (line[0] == '.') return -1;
    if (line[0] == '.' && line[1] == '.') return -1;
    return 1;
}

char *make_path(char *first, char *second) {
    char *path = malloc(sizeof(char) * (strlen(first) + strlen(second) + 2));
    sprintf(path, "%s/%s", first, second);
    return path;
}

int contains_string(char *file, char *to_find) {
    /**
     * 1 if true, -1 if false
     * **/
    //printf("SZUKAM : %s  w pliku : %s\n",  to_find, line);
    FILE *fp1 = fopen(file, "r");

    if (fp1) {
        while (feof(fp1) == 0) {
            char *line = find_line_lib(fp1);
            int i = 0;
            while (i < strlen(line)) {
                if (line[i] == to_find[0]) {
                    int j = i;
                    int k = 0;
                    while (k < strlen(to_find) && j < strlen(to_find)) {
                        if (line[j] == to_find[k]) {
                            k++;
                            j++;
                        } else break;
                    }
                    if (k == strlen(to_find)) {
                        return 1;
                    }
                }
                i++;
            }
        }
        return -1;
    }
    return -1;
}

int is_txt(char *line) {
    int line_len = strlen(line);
    if (line_len < 3) {
        return -1;
    }
    int i = line_len - 3;
    if (line[i] == 't' && line[i + 1] == 'x' && line[i + 2] == 't') {
        return 1;
    }
    return -1;
}

void search_dir(char *path, char *to_find, int max_depth, int curr_depth) {
    DIR *dp;
    struct dirent *entry;
    struct stat sb;
    pid_t child;
    dp = opendir(path);
    if (dp == NULL) {
        printf("error with opening directory: %s\n", path);
        exit(1);

    } else {
        entry = readdir(dp);
        while (entry != NULL) {
            if (lstat(make_path(path, entry->d_name), &sb) == -1) {
                printf("error with stat()\n");
                exit(1);
            }
            if (S_ISDIR(sb.st_mode) != 0 && is_non_backward(entry->d_name) == 1) {
                if (curr_depth < max_depth) {
                    child = fork();
                    if (child == 0) {
                        search_dir(make_path(path, entry->d_name), to_find, max_depth, curr_depth + 1);
                        exit(0);
                    }
                }
            } else {
                if (is_txt(entry->d_name) == 1 && contains_string(make_path(path, entry->d_name), to_find) == 1) {
                    printf("path: %s, file: %s, \n", path, entry->d_name);
                }
            }
            entry = readdir(dp);
        }
    }
    while(child = wait(NULL)){
        if(errno == ECHILD){
            break;
        }
    }
    closedir(dp);
}


int main(int argc, char **argv) {

    if (argc != 4) {
        printf("Check agument amount!\n");
        return 1;
    }


    search_dir(argv[1], argv[2], atoi(argv[3]), 0);
    return 0;
}
