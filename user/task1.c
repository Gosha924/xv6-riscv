#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    char arg = 'a';
    if (argc > 1 && argv[1][0] == 'b') {
        arg = 'b';
    }
    int pid;
    pid = fork();
    if (pid < 0) {
        printf("error\n");
        exit(1);
    }
    else if (pid == 0) {
        pause(50);
        exit(1);
    }
    else if (pid > 0) {
        printf("dady id: %d\n", getpid());
        printf("child id: %d\n", pid);
        if (arg == 'b') {
            kill(pid);
        }
        int status;
        int exit_id = wait(&status);
        printf("%d\n", exit_id);
        printf("status: %d\n", status);
        exit(0);
    }
    return 0;
}