#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
    int fd_a[2];
    int fd_b[2];
    int input_d;
    int n = 0;
    char buff[BUF_SIZE];
    char* input_path;

    if (argc < 2)
        input_path = "input";
    else
        input_path = argv[1];

    pipe(fd_a);
    pipe(fd_b);

    if (fork()==0) {
        if (fork()==0) {
            input_d = open(input_path, 0);
            close(fd_a[0]);
            close(fd_b[0]);
            close(fd_b[1]);

           while ((n=read(input_d, buff, BUF_SIZE)) != 0)
                write(fd_a[1], buff, n);
            close(fd_a[1]);
            close(input_d);
            exit(0);
        }
        else {
            close(fd_a[1]);
            close(fd_b[0]);
            while((n=read(fd_a[0], buff, BUF_SIZE)) != 0) {
                for (int i=0; i<n; i++)
                    buff[i] = toupper(buff[i]);
                write(fd_b[1], buff, n);
            }
            close(fd_b[1]);
            close(fd_a[0]);
            wait();
            exit(0);
        }
    }
    else {
        close(fd_b[1]);
        close(fd_a[0]);
        close(fd_a[1]);
        while ((n=read(fd_b[0], buff, BUF_SIZE)) != 0) 
            write(1, buff, n);
        
        close(fd_b[0]);
        close(1);
        wait();
        return 0;
        
    }
}