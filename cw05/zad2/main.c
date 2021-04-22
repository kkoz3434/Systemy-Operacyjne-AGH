#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_ordered(char *mode) {
    FILE *file;

    if (strcmp(mode, "date") == 0) {
        file = popen("echo | mail | sort -k 3", "r");
    } else {
        if (strcmp(mode, "sender") == 0) {
            file = popen("echo | mail -f | sort -k 2", "r");
        } else {
            return;
        }
    }

    char line[256];
    while (fgets(line, 256, file) != NULL) {
        printf("%s\n", line);
    }
}
void send_mail(char* mail, char* subject, char* content){
    FILE *file;
    char command[2048];
    sprintf(command,  "echo %s | mail -s '%s' %s", content, subject, mail);
    file = popen(command, "w");
    return;

}

int main(int argc, char *argv[]) {

    if (argc == 2) {
        // print ordered
        print_ordered(argv[1]);
    }
    if(argc ==4){
        char* mail = argv[1];
        char* subject = argv[2];
        char* text = argv[3];
        send_mail(mail, subject, text);
    }

    return 0;
}
