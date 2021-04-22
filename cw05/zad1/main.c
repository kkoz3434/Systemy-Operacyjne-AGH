#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>

#define MAX_ARGS 10  // max 10 args in command
#define MAX_LINE 256 // 256 chars in line
#define ALIASES_AMOUNT 10// max 10 pipes in one aliases

char *remove_white_spaces(char *str) {
    int i = 0, j = 0;
    while (str[i]) {
        if (str[i] != ' ')
            str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
    return str;
}

char *get_alias_name(char *line) {
    char *line_cpy = (char *) calloc(strlen(line), sizeof(char));
    strcpy(line_cpy, line);
    char *name = strtok(line_cpy, "=");
    name = remove_white_spaces(name);
    return name;
}

char **get_commands_arr(char *line) {
    //printf("GET CMDS FROM %s\n", line);
    char **commands = (char **) calloc(MAX_ARGS, sizeof(char *));
    char *r;
    char *line_cpy = (char *) calloc(strlen(line), sizeof(char));
    strcpy(line_cpy, line);
    r = strtok(line_cpy, "=");
    int i = 0;
    while ((commands[i] = strtok(NULL, "|")) != NULL) {
        commands[i][strcspn(commands[i], "\r\n")] = 0; // removes \r\n
        //commands[i] = remove_white_spaces(commands[i]);
        //printf("%s to parse\n", commands[i]);
        i++;
    }
    return commands;
}

char **parse_alias_names(char *line) {
    //printf("Starting to parse aliases to exec\n");
    char **aliases = (char **) calloc(ALIASES_AMOUNT, sizeof(char *));
    char *r;
    char *line_cpy = (char *) calloc(strlen(line), sizeof(char));
    strcpy(line_cpy, line);
    int i = 0;
    aliases[i] = strtok(line_cpy, "|");
    aliases[i] = remove_white_spaces(aliases[i]);
    i++;
    while ((aliases[i] = strtok(NULL, "|")) != NULL) {
        aliases[i][strcspn(aliases[i], "\r\n")] = 0; // removes \r\n
        aliases[i] = remove_white_spaces(aliases[i]);
        // printf("%s alias parse\n", aliases[i]);
        i++;
    }
    return aliases;
}

int *get_exec_indexes(char **to_find, char **names, int n) {
    int *result = (int *) calloc(ALIASES_AMOUNT, sizeof(int));
    int i = 0;
    int k = 0;
    while (to_find[i] != NULL) {
        for (int j = 0; j < n; ++j) {

            if (strcmp(to_find[i], names[j]) == 0) {
                result[k] = j;
                k++;
                break;
            }
        }
        i++;
    }
    result[k] = -1;
    return result;
}

char **get_single_command_argument(char *line) {
    char **commands = (char **) calloc(MAX_ARGS, sizeof(char *));
    char *r;
    char *line_cpy = (char *) calloc(strlen(line), sizeof(char));
    strcpy(line_cpy, line);
    r = strtok(line_cpy, " ");
    int i = 0;
    commands[i] = r;
    i++;
    while ((commands[i] = strtok(NULL, " ")) != NULL) {
        commands[i][strcspn(commands[i], "\r\n")] = 0; // removes \r\n
        //printf("%s to parse\n", commands[i]);
        i++;
    }
    commands[i] = NULL;
    return commands;
}

char *get_exec_name(char *line) {
    char *line_cpy = (char *) calloc(strlen(line), sizeof(char));
    strcpy(line_cpy, line);
    char *name = strtok(line_cpy, " ");
    name = remove_white_spaces(name);
    return name;
}

int main(int argc, char *argv[]) {
    printf("%d\n", argc);
    if (argc == 1) {
        //tests
    } else if (argc == 2) {
        char **names = (char **) calloc(ALIASES_AMOUNT, sizeof(char *));
        char ***commands = (char ***) calloc(ALIASES_AMOUNT, sizeof(char **));

        int current[2], prev[2];

        FILE *file = fopen(argv[1], "r");
        char *line = NULL;
        size_t readed;
        size_t len = 0;
        int counter = 0;

        while ((readed = getline(&line, &len, file)) != -1) {
            if (strstr(line, "=")) {
                //printf("Retrieved line of length %zu:\n", readed);
                //printf("%s", line);

                //parsujemy

                commands[counter] = get_commands_arr(line);
                //printf("in arr:%s\n", commands[counter][0]);

                names[counter] = get_alias_name(line);
                //printf("name %s\n", names[counter]);
                counter++;
            } else {
                // wykonujemy
                char **to_exec_names = parse_alias_names(line);
                int *to_exec_indexes = get_exec_indexes(to_exec_names, names, counter);



                for (int k = 0; to_exec_indexes[k] != -1; k++) {
                    int i = to_exec_indexes[k];
                    //printf("Program: %s\n", names[i]);
                    for (int j = 0; commands[i][j] != NULL; j++) {
                        // printf("Argumenty: %s\n", commands[i][j]);

                        char **args_arr = get_single_command_argument(commands[i][j]);
                        char *name_exec = get_exec_name(commands[i][j]);
                        printf("GONNA EXECUTE: %s\n", name_exec);
                        int l = 0;
                        while (args_arr[l] != NULL) {
                            printf("ARGUMENTS %d : %s\n", l, args_arr[l]);
                            l++;
                        }

                        //close(current[0]);
                        pipe(current);
                        pid_t pid = fork();

                        if (pid == 0) {
                            printf("I'm in child!\n");
                            //close(current[0]);   //tu pierwszy raz 1


                            if ((k == 0 && j == 0)) {
                                // pierwszy przypadek
                                //close(current[0]);    POWTORZENIE1
                                printf("##1##\n");
                                if (!(to_exec_indexes[k + 1] == -1 && commands[i][j + 1] == NULL)){
                                    printf("##2##\n");
                                    dup2(current[1], STDOUT_FILENO);
                                }
                            } else if (to_exec_indexes[k + 1] == -1 && commands[i][j + 1] == NULL) {
                                //drugi przypadek
                                //close(current[0]);    POWTORZENIE1
                                close(current[1]);
                                //close(prev[1]);
                                dup2(prev[0], STDIN_FILENO);
                                printf("##3##\n");
                            } else {
                                //trzeci przypadek
                                //close(prev[1]);
                                //close(current[0]);     POWTORZENIE1
                                dup2(prev[0], STDIN_FILENO);
                                dup2(current[1], STDOUT_FILENO);
                                printf("##4##\n");
                            }



                            printf("EXECUTE: %s\n", name_exec);
                            if (execvp(name_exec, args_arr) == -1) {
                                printf("Exec error :c \n");
                                exit(1);
                            }
                            exit(0);
                        }


                        prev[0] = current[0];
                        prev[1] = current[1];

                         if (pipe(current) != 0) {
                             printf("Can't change pipe!\n");
                             exit(1);
                         }

                    }
                }
            }
        }


        fclose(file);
    } else {
        printf("Wrong amount of arguments!\n");
        exit(1);
    }


}