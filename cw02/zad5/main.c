#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

double seconds(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

int is_line(char *line, int size) {
    for (int i = 0; i < size; ++i) {
        if (line[i] == '\n') {
            return i + 1;
        }
    }
    return -1;
}

void break_line_lib(char *line, FILE *out) {
    int line_len = strlen(line);
    int count = 0;
    char new_line = '\n';
    int i = 0;
    while (i < line_len) {
        if (count < 50) {
            fwrite(line, sizeof(char), 1, out);
            count++;
            i++;
        } else {
            fwrite(&new_line, sizeof(char), 1, out);
            count = 0;
        }
    }
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

void lib_time(char *file1, char *w_file) {

    // time functions

    struct tms *start = malloc(sizeof(struct tms));
    struct tms *end = malloc(sizeof(struct tms));
    clock_t start_time;
    clock_t end_time;

    int even_counter = 0;

    //printf("lib_time running \n");
    start_time = times(start);
    FILE *fp1 = fopen(file1, "r");

    FILE *out = fopen(w_file, "w");


    if (fp1) {
        while (feof(fp1) == 0) {
            char *line = find_line_lib(fp1);
            break_line_lib(line, out);
            free(line);
        }
        // dla dynamicznie alokowanej byl problem edit: snprintf :))

        fclose(out);
        fclose(fp1);

        end_time = times(end);

        printf("Czas dla funckji bibliotecznej: \n");
        printf("real time:  %lf\n", seconds(start_time, end_time));
        printf("user time:  %lf\n", seconds(start->tms_utime, end->tms_utime));
        printf(" sys time:  %lf\n", seconds(start->tms_stime, end->tms_stime));
    } else {
        printf("nie mozna otworzyc pliku [in lib_time function]");
        return;
    }
}

void break_line_sys(char *line, int out_descryptor) {
    int line_len = strlen(line);
    int count = 0;
    char new_line = '\n';
    int i = 0;
    while (i < line_len) {
        if (count < 50) {
            write(out_descryptor, line, 1);
            count++;
            i++;
        } else {
            write(out_descryptor, &new_line, 1);
            count = 0;
        }
    }
}

char *find_line_sys(int descryptor) {
    //printf("1");

    size_t tmp_size = 256;
    char *tmp_string = (char *) calloc(tmp_size, sizeof(char));
    size_t readed_lines = read(descryptor, tmp_string, tmp_size);
    int have_line = is_line(tmp_string, readed_lines);

    //printf("2");

    while (have_line == -1 && readed_lines == tmp_size) {
        lseek(descryptor, -readed_lines, SEEK_CUR); //cofamy sie
        tmp_size *= 2;
        free(tmp_string);
        tmp_string = (char *) calloc(tmp_size, sizeof(char));

        readed_lines = read(descryptor, tmp_string, tmp_size);
        have_line = is_line(tmp_string, readed_lines);
    }

    //printf("3");

    char *result;
    if (readed_lines == 0) { // for while loop in function measuring time
        result = NULL;
        //printf("4");
    } else if (have_line == -1) { //end of file
        result = (char *) calloc(readed_lines + 2, sizeof(char));
        strncpy(result, tmp_string, readed_lines);
        result[readed_lines] = '\0';
        //printf("5");
    } else {
        result = (char *) calloc(have_line + 2, sizeof(char));
        strncpy(result, tmp_string, have_line);
        result[have_line] = '\0';
        lseek(descryptor, -(readed_lines - have_line), SEEK_CUR); // <-- 2 godziny rozpaczy
        //printf("6");
    }
    free(tmp_string);
    return result;
}


void sys_time(char *file1, char *w_file) {

    // time functions

    struct tms *start = malloc(sizeof(struct tms));
    struct tms *end = malloc(sizeof(struct tms));
    clock_t start_time;
    clock_t end_time;

    //printf("sys_time running\n");

    start_time = times(start);
    int fp1 = open(file1, O_RDONLY);

    int out = open(w_file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);



    //printf("sys_time opening\n");

    int even_counter = 0;
    if (fp1 != -1) {
        char *line;
        //printf("sys_time findline\n");
        line = find_line_sys(fp1);
        //printf("sys_time findline2\n");
        while (line != NULL) {
            break_line_sys(line, out);
            line = find_line_sys(fp1);
        }
        close(fp1);
        close(out);

        end_time = times(end);


        printf("\n Czas dla funckji systemowych \n");
        printf("real time:  %lf\n", seconds(start_time, end_time));
        printf("user time:  %lf\n", seconds(start->tms_utime, end->tms_utime));
        printf(" sys time:  %lf\n", seconds(start->tms_stime, end->tms_stime));
    } else {
        printf("nie mozna otworzyc pliku [in sys_time function]");
        return;
    }
}


int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Zla liczba argumentów!\n");
    } else {
        sys_time(argv[1], argv[2]);
        printf("\n######\n");
        lib_time(argv[1], argv[2]);
    }
}