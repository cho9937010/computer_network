/*
    Computer Network Term Project
    Mini Cloud Storage

    client.c
    2018/05/20
    Cho Jeong Jae, Jeong Min Hyuk, Kim Hyeon Jin, Lee Sang Yeop
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT            5883
#define ID_LEN          4
#define PATH_BUFLEN     30
#define PARAM_BUFLEN    40
#define CLOUD_ROOT      "/home/cho99/Cloud/" // modify for server environment

typedef enum { LIST, UPLOAD, DOWNLOAD, REMOVE, SHARE, EXIT } command_t;

void error_handling(char *message);


int main()
{
    int server_sock;
    int client_sock;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addrlen;

    int pid;
    char id[ID_LEN + 1] = { 0 };
    char path[PATH_BUFLEN] = { 0 };
    char param[PARAM_BUFLEN] = { 0 };

    struct stat st;
    command_t command;

    // socket()
    if ((server_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_handling("socket() error");

    // set server_addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // bind(), listen()
    if (bind(server_sock , (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        error_handling("bind() error");
    if (listen(server_sock, 5) == -1)
        error_handling("listen() error");

    // start accepting loop
    while (1) {
        // accept()
        if ((client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addrlen)) == -1)
            error_handling("accept() error");

        // print ip and fd
        fputs(inet_ntoa(client_addr.sin_addr), stdout);
        printf(" connected to %d\n", client_sock);

        // fork()
        if ((pid = fork()) == -1)
            error_handling("fork() error");

        // parent process -> repeat accept()
        // child process -> do below
        /**** child process start ****/
        if (pid == 0) {
            // receive ID  (ID consists of 4 alphabets)
            if (read(client_sock, id, ID_LEN) <= 0) {
                // return value 0 -> client_sock is closed now
                // return value -1 -> error
                close(client_sock);
                exit(0);
            }
            printf("[%s] fd: %d\n", id, client_sock);

            // access user's directory
            strcpy(path, CLOUD_ROOT);
            strcat(path, id);

            if (stat(path, &st) == -1)  // if path dose not exist
              mkdir(path, 0777);
            chdir(path);

            // current working directory
            printf("[%s] path: %s\n", id, getcwd(NULL, 0));

            // start commands receiving loop
            while (1) {
                // read a command
                if (read(client_sock, &command, 4) <= 0) {
                    close(client_sock);
                    exit(0);
                }

                printf("[%s] ", id);

                // exit branch
                if (command == EXIT) {
                    printf("[%s] user exit\n", id);
                    break;
                }

                // command switching
                switch (command) {
                case LIST:   // ls
                    printf("ls\n");
                    /*
                    to do:
                        transfer (encoded?) file list to client
                    */
                    break;
                case UPLOAD:   // upload
                    printf("upload\n");

                    read(client_sock, param, PARAM_BUFLEN);
                    printf("%s\n",param);
                    /*
                    to do:
                        get(read) the file from client
                        (open file, read from socket, write to file)
                    */
                    break;
                case DOWNLOAD:   // download
                    read(client_sock, param, PARAM_BUFLEN);
                    printf("download %s\n", param);
                    /*
                    to do:
                        check existence of the received parameter
                        transfer(write) the file to client
                    */
                    break;
                case SHARE:   // share
                    read(client_sock, param, PARAM_BUFLEN);
                    printf("share %s\n", param);
                    /*
                    to do:
                        check existence of the received parameter
                        make a hard link at the shared directory (?)
                    */
                    break;
                case REMOVE:   // remove
                    read(client_sock, param, PARAM_BUFLEN);
                    printf("remove %s\n", param);
                    /*
                    to do:
                        check existence of the received parameter
                        remove the file
                    */
                    break;
                default :
                    printf("invalid command\n");
                    break;
                }
            }

            // child process terminates
            close(client_sock);
            return 0;
        }
        /**** child process end ****/
    }
}

void error_handling(char *message)
{
    perror(message);
    exit(1);
}
