#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>


int count_lines(FILE *f) {
    fseek(f, 0, SEEK_SET); // <- zjadło mnóstwo nerwów
    size_t len = 0;
    char *line = NULL;

    int counter = 0;
    while ((getline(&line, &len, f)) != -1) {
        counter++;
    }

    fseek(f, 0, SEEK_SET);
    return counter;
}

char **merge_two(char *name1, char *name2) {
    FILE *f1, *f2;

    f1 = fopen(name1, "r");
    f2 = fopen(name2, "r");

    if (f1 == NULL)
        exit(EXIT_FAILURE);

    int index = 0;
    if (f2 == NULL) {
        int size = count_lines(f1);
        char **pair = calloc(sizeof(char *), (size));

        size_t line_length = 0; // variable for using getline
        while (index < size) {
            getline(&pair[index], &line_length, f1);
            printf("%s", pair[index]);
            index++;
        }
        return pair;

    } else {
        int size1 = count_lines(f1);
        int size2 = count_lines(f2);

        int min_size = size1 < size2 ? size1 : size2;
        char **pair = calloc(sizeof(char *), (size1 + size2 + 1));
        pair[size1 + size2] = NULL;

        size_t line_length = 0; // variable for using getline
        while (index < 2 * min_size) {
            if (index % 2 == 0) {
                getline(&pair[index], &line_length, f1);
                printf("%s", pair[index]);
            } else {
                getline(&pair[index], &line_length, f2);
                printf("%s", pair[index]);
            }
            index += 1;
        }

        if (size1 > size2) {
            while (index < size1 + size2) {
                getline(&pair[index], &line_length, f1);
                printf("%s", pair[index]);
                index++;
            }
        } else {
            while (index < size1 + size2) {
                getline(&pair[index], &line_length, f2);
                printf("%s", pair[index]);
                index++;
            }
        }

        fclose(f1);
        fclose(f2);
        return pair;

    }
}

FILE* merge_two_tmp_file(char* name1, char* name2){
    FILE *f1, *f2;

    f1 = fopen(name1, "r");
    f2 = fopen(name2, "r");
    FILE* tmp = tmpfile();
    int index = 0;
    char* to_insert = NULL;

    if (f2 == NULL) {
        int size = count_lines(f1);
        size_t line_length = 0; // variable for using getline
        while (index < size) {
            getline(&to_insert, &line_length, f1);
            fputs(to_insert, tmp);
            index++;
        }
        return tmp;

    } else {
        int size1 = count_lines(f1);
        int size2 = count_lines(f2);

        int min_size = size1 < size2 ? size1 : size2;

        size_t line_length = 0; // variable for using getline
        while (index < 2 * min_size) {
            if (index % 2 == 0) {
                getline(&to_insert, &line_length, f1);
            } else {
                getline(&to_insert, &line_length, f2);
            }
            fputs(to_insert, tmp);
            index += 1;
        }

        if (size1 > size2) {
            while (index < size1 + size2) {
                getline(&to_insert, &line_length, f1);
                fputs(to_insert, tmp);
                index++;
            }
        } else {
            while (index < size1 + size2) {
                getline(&to_insert, &line_length, f2);
                fputs(to_insert, tmp);
                index++;
            }
        }

        fclose(f1);
        fclose(f2);
        return tmp;
    }
}

void merge_two_tmp_file_void(char* name1, char* name2){
    FILE *f1, *f2;

    f1 = fopen(name1, "r");
    f2 = fopen(name2, "r");
    FILE* tmp = tmpfile();
    int index = 0;
    char* to_insert = NULL;

    if (f2 == NULL) {
        int size = count_lines(f1);
        size_t line_length = 0; // variable for using getline
        while (index < size) {
            getline(&to_insert, &line_length, f1);
            fputs(to_insert, tmp);
            index++;
        }
        return;

    } else {
        int size1 = count_lines(f1);
        int size2 = count_lines(f2);

        int min_size = size1 < size2 ? size1 : size2;

        size_t line_length = 0; // variable for using getline
        while (index < 2 * min_size) {
            if (index % 2 == 0) {
                getline(&to_insert, &line_length, f1);
            } else {
                getline(&to_insert, &line_length, f2);
            }
            fputs(to_insert, tmp);
            index += 1;
        }

        if (size1 > size2) {
            while (index < size1 + size2) {
                getline(&to_insert, &line_length, f1);
                fputs(to_insert, tmp);
                index++;
            }
        } else {
            while (index < size1 + size2) {
                getline(&to_insert, &line_length, f2);
                fputs(to_insert, tmp);
                index++;
            }
        }

        fclose(f1);
        fclose(f2);
    }
}

void insert_into_main_arr(char*** arr, FILE* tmp, int arr_size){
    rewind(tmp); // tego szukałem godzinę :)
    int counter=0;
    while(arr[counter] != NULL){
        counter++;
    }

    int size = count_lines(tmp);
    char** arr_tmp= calloc(sizeof(char*), size + 1);
    arr_tmp[size] = NULL;
    size_t line_length =0;
    int i=0;
    while(i<size){
        getline(&arr_tmp[i], &line_length, tmp);
        i++;
    }
    int index=0;
    while(index<arr_size && arr[index]!=NULL){
        index++;
    }
    if(index<arr_size){
        arr[index] = arr_tmp;
    }
}
char ***create_table(int argc){
    char ***result = calloc(sizeof(char **), argc / 2 + 1);
    result[argc / 2] = NULL;
    for (int i = 0; i < argc/2; i++) {
        result[i]=NULL;
    }
    return result;
}

char ***merge_all(int argc, char **args) {
    char ***result = create_table(argc);
    int index = 0;
    int i = 0;
    while (i < argc) {
        FILE* block =  merge_two_tmp_file(args[i+1], args[i+2]);
        insert_into_main_arr(result,block,argc);
        i += 2;
        index++;
    }
    return result;
}

void print_block(char **arr) {
    int i = 0;

    while (arr[i]!=NULL){
        printf("element: %d, to: %s\n", i, arr[i]);
        i++;
    }
}

void print_all(char ***arr) {
    int i = 0;
    while (arr[i] != NULL) {
        printf("POSZLO %d\n", i);
        print_block(arr[i]);
        i++;
        printf("----------------\n");
    }
}

void delete_line(char **arr, int to_delete) {
    int i = to_delete;
    free(arr[i]);
    while (arr[i] != NULL) {
        arr[i] = arr[i + 1];
        i++;
    }
}

void delete_pair(char ***arr, int to_delete) {
    int i = to_delete;
    int j = 0;
    while (arr[i][j] != NULL) {
        free(arr[i][j]);
        j++;
    }
    free(arr[i]);
    while (arr[i] != NULL) {
        arr[i] = arr[i + 1];
        i++;
    }

}