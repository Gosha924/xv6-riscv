#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int a;
    a = sum(1, 3);
    printf("sum: %d\n", a);
    exit(0);

}