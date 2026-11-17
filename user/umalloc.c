#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"


typedef struct header {
  uint64 size;
  uint64 is_free;
  struct header* next;
} header_t;

static header_t *heap_start;
static header_t *heap_end;

void* malloc(uint64 size) {
  void *ptr = sbrk(size);
  if (ptr == (void*)-1) {
      return 0;
  }
  return ptr;
}
