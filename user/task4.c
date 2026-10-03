#include "kernel/procinfo.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    procinfo_type info;

    info.id = 0;
    info.state = 0;
    info.parent_id = 0;
    info.name[0] = '\0';

    exit(0);
}