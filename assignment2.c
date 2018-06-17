#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#define BUF_SIZE 1024
#define ARG_SIZE 10

char buff[BUF_SIZE];

int main(void) {
    int fd[2];
    pid_t pid;
    pid_t bg_pid;
    pid_t wait_pid;
    int status;

    int login=1;
    int i = 1; //number of argument
    int output;
    int n; //for pipeline
    
    int stdin_copy = dup(0);
    int stdout_copy = dup(1);

    char* token = " \t\n";
    char* arg[ARG_SIZE];
    char* tempstr;
    
    while (login) {

        i = 1;
        printf("zbshell $ ");
        memset(buff, 0, BUF_SIZE);
        fgets(buff, BUF_SIZE, stdin);
        tempstr = strtok(buff,token);
        arg[0] = (char *)malloc(sizeof(char)*(strlen(tempstr)+1));
        strcpy(arg[0], tempstr);
        while ((tempstr = strtok(NULL, token)) != NULL) {
            arg[i] = (char *)malloc(sizeof(char)*(strlen(tempstr)+1));
            strcpy(arg[i], tempstr);
            i++;
            if (i>=10) break;
        }

        
        
        if (strcmp(arg[0],"exit")==0) {
            login = 0;
            exit(0);
        }
        else {
            if ((i > 1) && (strcmp(arg[1], ">") == 0)) {
                pipe(fd);
                pid = fork();
                if (pid == 0) {
                    close(1);
                    close(fd[0]);
                    dup(fd[1]);
                    close(fd[1]);
                    if (execlp(arg[0], NULL)==-1)
                        printf("undefined command\n");
                    exit(0);
                }
                else {
                    memset(buff, 0, BUF_SIZE);
                    close(0);
                    close(fd[1]);
                    dup(fd[0]);
                    output = creat(arg[2], 0666);
                    while ((n=read(fd[0], buff, BUF_SIZE)) != 0)
                        write(output, buff, n);
                    close(output);
                    close(fd[0]);
                    while(pid!=wait(&status));

                    dup2(stdin_copy, 0);
                    dup2(stdout_copy, 1); //we have to re-open stdio
                }   
            }
            else if ((arg[0][strlen(arg[0])-1])=='&') {
                //background
                arg[0][strlen(arg[0])-1] = '\0';
                pid = fork();
                if (pid==0) {
                    login = 0;
                    if (execlp(arg[0], NULL)==-1)
                        printf("undefined command\n");
                    exit(0);
                }
                else {
                    printf("pid : %d\n", pid); 
                    bg_pid = pid;
                 }
            }
            else {
                //foreground
                pid = fork();
                if (pid==0) {
                    login = 0;
                    if (execlp(arg[0], NULL)==-1)
                        printf("undefined command\n");
                    exit(0);
                }
                else  {
                    printf("pid = %d\n", pid);
                    while(1) {
                        wait_pid = wait(&status);
                        if (wait_pid == bg_pid)
                            printf("%d is terminated.\n", bg_pid);
                        else if (wait_pid == pid)
                            break;
                    };
                }
            }
        }
        
    }

    return 0;
}