/*
    Computer Network Term Project
    Mini Cloud Storage

    client.c
    2018/05/20
    Cho Jeong Jae, Jung Min Hyeok, Kim Hyeon Jin, Lee Sang Yeop
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP       "127.0.0.1"
#define PORT            5883
#define ID_LEN          4
#define COMMAND_BUFLEN  50
#define PARAM_BUFLEN    40

typedef enum { LIST, UPLOAD, DOWNLOAD, REMOVE, SHARE, EXIT } command_t;

command_t str_to_command(char *str);
int input_str(char *strbuf, int buflen);
void error_handling(char *str);


int main()
{
    int i;
    int sock;
    struct sockaddr_in server_addr;

    char id[ID_LEN + 1] = { 0 };
    char command_str[COMMAND_BUFLEN] = { 0 };
    char param[PARAM_BUFLEN] = { 0 };
    command_t command;

    // create socket
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_handling("sock() error");

    // set server_addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // connect to server
    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        error_handling("connect() error");

    // get an id and transfer it to server
    fputs("ID: ", stdout);

    while (input_str(id, ID_LEN + 1) == 0); // forbid null id
    write(sock, id, 4);

    // start command line loop
    while (1) {
        // get a command
        fputs(">>> ", stdout);
        input_str(command_str, COMMAND_BUFLEN);

        // distinguish command from parameter
        for (i = 0; command_str[i] != '\0'; i++) {
            if (command_str[i] == ' ') {
                command_str[i++] = '\0';
                break;
            }
        }
        memset(param, 0, PARAM_BUFLEN);
        for (; command_str[i] != '\0'; i++) {
            if (command_str[i] != ' ') {
                strcpy(param, &command_str[i]);
                break;
            }
        }

        command = str_to_command(command_str);

        // exit branch
        if (command == EXIT) {
            write(sock, &command, 4);
            break;
        }

        // command switching
        switch (command) {
        case LIST:
            write(sock, &command, 4);
            /*
            to do:
                get file list from server
                print file list
            */
            puts("file1\nfile2\nfile3\n..."); // printing example
            break;
        case UPLOAD:
            fputs("업로드할 파일명 입력 :",stdout);
            input_str(param,10);
            /*
            to do:
                check existence of the parameter
            */
            write(sock, &command, 4);

            write(sock, param, 10);
            /*
            to do:
                upload file to server
                (open param_str,
                read from the file,
                write to socket)
            */
            break;
        case DOWNLOAD:
            if (strlen(param) > 0) {
                write(sock, &command, 4);
                write(sock, param, PARAM_BUFLEN);
                /*
                to do:
                    download file from server
                */
            } else {
                puts("Need file name");
            }
            break;
        case REMOVE:
        case SHARE:
            if (strlen(param) > 0) {
                write(sock, &command, 4);
                write(sock, param, PARAM_BUFLEN);
            } else {
                puts("Need file name");
            }
            break;
        default:
            puts("Invalid command");
        }
    }

    // program terminates
    close(sock);
    return 0;
}

command_t str_to_command(char *str) // (char *) to (command_t); for easy progress
{
    if (!strcmp(str, "list") || !strcmp(str, "ls")) {
        return LIST;
    } else if (!strcmp(str, "upload") || !strcmp(str, "up")) {
        return UPLOAD;
    } else if (!strcmp(str, "download") || !strcmp(str, "down")) {
        return DOWNLOAD;
    } else if (!strcmp(str, "remove") || !strcmp(str, "rm")) {
        return REMOVE;
    } else if (!strcmp(str, "share")) {
        return SHARE;
    } else if (!strcmp(str, "exit") || !strcmp(str, "quit")) {
        return EXIT;
    } else {
        return -1;
    }
}

int input_str(char *strbuf, int buflen) // get a string with flush and without newline character
{
    fgets(strbuf, buflen, stdin);

    if (strbuf[strlen(strbuf) - 1] == '\n')
        strbuf[strlen(strbuf) - 1] = '\0';
    else
        while (getchar() != '\n');

    return strlen(strbuf);
}

void error_handling(char *message)
{
    perror(message);
    exit(1);
}
