#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/procinfo.h"

int
main(int argc, char *argv[])
{
  int cap = 16;
  struct procinfo *buf;
  int n;

  for (;;) {
    buf = malloc(cap * sizeof(struct procinfo));
    if (buf == 0) {
      fprintf(2, "ps: malloc failed\n");
      exit(1);
    }

    n = ps_listinfo(buf, cap);
    if (n < 0) {
      fprintf(2, "ps: ps_listinfo failed\n");
      free(buf);
      exit(1);
    }

    if (n <= cap)
      break;

    free(buf);
    cap = n;
  }

  printf("pid\tname\t\tstate\tppid\n");
  for (int i = 0; i < n; i++) {
    printf("%d\t%s\t\t%d\t%d\n",
           buf[i].id, buf[i].name, buf[i].state, buf[i].parent_id);
  }

  free(buf);
  exit(0);
}