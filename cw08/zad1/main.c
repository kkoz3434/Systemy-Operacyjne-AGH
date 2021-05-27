#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/times.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define NAME_LEN 256
#define BLOCK 0
#define NUMBERS 1
#define MAX_COLOR 255

int thr_amount;
char input_file_name[NAME_LEN];
char output_file_name[NAME_LEN];
int method;
int height;
int width;
char *magic_number;
int max_color;
int **input;
int **output;


typedef struct thread_info {
    pthread_t thr_id;
    int begin;
    int end;
    long int seconds;
    long int miliseconds;
};

struct thread_info *get_thread_info_array() {
    struct thread_info *thr_info = calloc(thr_amount, sizeof(struct thread_info));
    if (method == NUMBERS) {
        for (int i = 0; i < thr_amount; ++i) {
            thr_info[i].begin = (int) (MAX_COLOR / thr_amount) * i;
            thr_info[i].end = (int) (MAX_COLOR / thr_amount) * (i + 1);
        }
        thr_info[thr_amount - 1].end = MAX_COLOR;

    } else if (method == BLOCK) {
        for (int i = 0; i < thr_amount; ++i) {
            thr_info[i].begin = (int) (width / thr_amount) * i;
            thr_info[i].end = (int) (width / thr_amount) * (i + 1);
        }
        thr_info[thr_amount - 1].end = width;
    }
    return thr_info;
}

void *thread_function(void *arg) {
    // get the info from thread creation arg list
    struct thread_info *info;
    info = (struct thread_info *) arg;

    //https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c
    struct timeval tval_before, tval_after, tval_result;

    gettimeofday(&tval_before, NULL);

    // values of pixels from A to B
    if (method == NUMBERS) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (info->begin <= input[i][j] && info->end > input[i][j]) {
                    output[i][j] = MAX_COLOR - input[i][j];
                }
            }
        }
    }
    // strap of pixels on X axis
    if (method == BLOCK) {
        // soon
        for (int i = 0; i < height; ++i) {
            for (int j = info->begin; j < info->end; ++j) {
                output[i][j] = MAX_COLOR - input[i][j];
            }
        }
    }

    //time and updating the structs
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    info->seconds = (long int) tval_result.tv_sec;
    info->miliseconds = (long int) tval_result.tv_usec;

    pthread_exit(NULL);
}

void get_image(FILE *input_image) {// check if filetype is correct
    int readed =0;
    getline(&magic_number, &readed, input_image);
    if (magic_number[0] != 'P' || magic_number[1] != '2') {
        printf("Wrong magic number\n");
        exit(-4);
    }

    // read width, height and color max value
    fscanf(input_image, "%d", &width);
    fscanf(input_image, "%d", &height);
    fscanf(input_image, "%d", &max_color);

    input = calloc(height, sizeof(int *));
    output = calloc(height, sizeof(int *));

    for (int i = 0; i < height; ++i) {
        //allocate the memory for arrays of int -> our image
        input[i] = calloc(width, sizeof(int));
        output[i] = calloc(width, sizeof(int));
        for (int j = 0; j < width; ++j) {
            fscanf(input_image, "%d", &input[i][j]);
        }
    }
    fclose(input_image);
}



int main(int argc, char **argv) {


    if (argc != 5) {
        printf("Wrong amount of arguments!\n");
        exit(-1);
    }

    //read the args
    thr_amount = atoi(argv[1]);
    printf("%s\n",argv[2]);
    if (strcmp(argv[2], "numbers")==0) {
        method = NUMBERS;
    } else if (strcmp(argv[2], "block")==0) {
        method = BLOCK;
    } else {
        printf("Wrong method (numbers / block) \n");
        exit(-2);
    }


    strcpy(input_file_name, argv[3]);
    strcpy(output_file_name, argv[4]);


    //open the input file

    FILE *input_image = fopen(input_file_name, "r");
    if (input_image == NULL) {
        printf("Error with reading a file\n");
        exit(-3);
    }

    // do the rest with image (validation and reading)
    get_image(input_image);


    //create array of structs threads_info
    struct thread_info *thr_info = get_thread_info_array();

    //measure time
    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL);


    for (int i2 = 0; i2 < thr_amount; ++i2) {
        // create thread with arguments
        pthread_create(&thr_info[i2].thr_id,
                       NULL, &thread_function, &thr_info[i2]);
    }

    //waiting for threads
    for (int i = 0; i < thr_amount; ++i) {
        pthread_join(thr_info->thr_id, NULL);
    }
    //stop the clock
    gettimeofday(&tval_after, NULL);


    //open output file or create
    FILE *output_image = fopen(output_file_name, "w");
    if (output_image == NULL) {
        printf("Error with writing a file\n");
        exit(-4);
    }

    // writing the header of image
    fprintf(output_image, "%s", magic_number);
    fprintf(output_image, "%d ", width);
    fprintf(output_image, "%d\n", height);
    fprintf(output_image, "%d\n", MAX_COLOR);

    //writing the rest -> arr of colors
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fprintf(output_image, "%d ", output[i][j]);
        }
        fprintf(output_image, "\n");
    }

    //close file
    fclose(output_image);


    // time for printing time :)
    timersub(&tval_after, &tval_before, &tval_result);

    if(method == NUMBERS){
        printf("Method: NUMBERS \n");
    }
    if(method ==BLOCK){
        printf("Method: BLOCK\n");
    }

    printf("Threads amount: %d\n", thr_amount);
    printf("## PRINTING THREADS TIMES:\n");
    for (int i = 0; i < thr_amount; ++i) {
        printf("%d Thread_ID : %d, time: %f\n",i,
               (int)thr_info[i].thr_id, (float)(thr_info[i].seconds +  thr_info[i].miliseconds*1e-6));
    }

    printf("Total time %f\n",
           (float)( tval_result.tv_sec +  tval_result.tv_usec*1e-6));
    printf("\n\n");

    return 0;
}



