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

int have_decimal07(int number){
    if(number<10){
        return 0;
    }
    if((number/10)%10==7 || (number/10)%10==0) {
        return 1;
    }
    return 0;
}

int is_square_of_int(int number){
    double sqr = sqrt((double) number);
    int sqr_int = (int) sqr;
    if(sqr == sqr_int){
        return 1;
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
        result=(char*)calloc(have_line + 2, sizeof(char));
        strncpy(result, tmp_string, have_line);
        fseek(fp, -(readed-have_line), SEEK_CUR); // <-- 2 godziny rozpaczy
        result[have_line] = '\0';
    }
    free(tmp_string);
    return result;
}

void lib_time(char *file1) {

    // time functions

    struct tms *start = malloc(sizeof(struct tms));
    struct tms *end = malloc(sizeof(struct tms));
    clock_t start_time;
    clock_t end_time;

    int even_counter = 0;

    //printf("lib_time running \n");
    start_time = times(start);
    FILE *fp1 = fopen(file1, "r");

    FILE* out1 = fopen("a.txt", "w");
    FILE* out2 = fopen("b.txt", "w");
    FILE* out3 = fopen("c.txt", "w");

    if (fp1 ) {
        while (feof(fp1) == 0) {
                char *line = find_line_lib(fp1);
                int line_int = atoi(line);
                if(line_int%2==0){
                    //printf("%s\n",line);
                    even_counter++;
                }
                if(have_decimal07(line_int)==1){
                    //printf("%s\n",line);
                    fwrite(line, sizeof(char), strlen(line), out2);
                }
                if(is_square_of_int(line_int)==1){
                    //printf("%s\n",line);
                    fwrite(line, sizeof(char), strlen(line), out3);
                }
                free(line);
        }
        // dla dynamicznie alokowanej byl problem edit: snprintf :))
        char out1_str[256] = "Liczb parzystych jest ";
        char number_str[256];
        sprintf(number_str, "%d\n", even_counter);
        strcat(out1_str, number_str);
        fwrite(out1_str, sizeof(char), strlen(out1_str), out1);

        fclose(out1);
        fclose(out2);
        fclose(out3);
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
    if(readed_lines == 0){ // for while loop in function measuring time
        result = NULL;
        //printf("4");
    }
    else if (have_line == -1) { //end of file
        result = (char *) calloc(readed_lines + 2, sizeof(char));
        strncpy(result, tmp_string, readed_lines);
        result[readed_lines] = '\0';
        //printf("5");
    } else {
        result=(char*)calloc(have_line + 2, sizeof(char));
        strncpy(result, tmp_string, have_line);
        result[have_line] = '\0';
        lseek(descryptor, -(readed_lines-have_line), SEEK_CUR); // <-- 2 godziny rozpaczy
        //printf("6");
    }
    free(tmp_string);
    return result;
}

void sys_time(char *file1) {

    // time functions

    struct tms *start = malloc(sizeof(struct tms));
    struct tms *end = malloc(sizeof(struct tms));
    clock_t start_time;
    clock_t end_time;

    //printf("sys_time running\n");

    start_time = times(start);
    int  fp1 = open(file1, O_RDONLY);

    int out1 = open("a.txt", O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
    int out2 = open("b.txt", O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
    int out3 = open("c.txt", O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);


    //printf("sys_time opening\n");

    int even_counter = 0;
    if (fp1 != -1) {
        char* line;
        //printf("sys_time findline\n");
        line = find_line_sys(fp1);
        //printf("sys_time findline2\n");
        while (line != NULL ) {
            int line_int = atoi(line);
            if(line_int%2==0){
                //printf("%s\n",line);
                even_counter++;
            }
            if(have_decimal07(line_int)==1){
                //printf("%s\n",line);
                write(out2, line, strlen(line));
            }
            if(is_square_of_int(line_int)==1){
                //printf("%s\n",line);
                write(out3, line, strlen(line));
            }
            line = find_line_sys(fp1);
        }


        char out1_str[256] = "Liczb parzystych jest ";
        char number_str[256];
        sprintf(number_str, "%d\n", even_counter);
        strcat(out1_str, number_str);
        write(out2, out1_str, strlen(out1_str));

        close(fp1);
        close(out1);
        close(out2);
        close(out3);

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

    if(argc != 2){
        printf("Zla liczba argumentów!\n");
    }
    else{
        sys_time(argv[1]);
        printf("\n######\n");
        lib_time(argv[1]);
    }
}