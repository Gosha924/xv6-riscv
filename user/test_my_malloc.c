#include "kernel/types.h"
#include "user/user.h"

int main() {
    void *a = malloc(100);

    void *b = malloc(75);

    void *c = malloc(50);
    void *d = malloc(150);

    free(a);
    free(b);
    free(c);
    free(d);
    exit(0);
}