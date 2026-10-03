#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

int main(int argc, char *argv[]) 
{
    int first_pipe[2];
    if (pipe(first_pipe) < 0) {
        printf("ERROR: cant create pipe\n");
        exit(1);
    }
    int pid;
    pid = fork();
    if (pid < 0) {
        printf("ERROR: cant create process\n");
        exit(1);
    }
    else if (pid == 0) {
        close(first_pipe[WRITE]);
        close(0);
        dup(first_pipe[0]);
        close(first_pipe[READ]);
        char *argv[] = {"/wc", 0};
        exec("/wc", argv);
    }
    else {
        close(first_pipe[READ]);
        for (int i = 1; i < argc; i++) {
            write(first_pipe[1], argv[i], strlen(argv[i]));
            write(first_pipe[1], "\n", 1);
        }
        close(first_pipe[WRITE]);
        wait((int *) 0);
        exit(0);
    }
    return 0;
}