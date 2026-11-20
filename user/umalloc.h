#ifndef USER_UMALLOC_H
#define USER_UMALLOC_H

#include "kernel/types.h"

#define SIZEHEADER sizeof(Header)
#define NUMBINS 8

typedef struct header {
  uint64 size;
  uint64 is_free;
  struct header* next_free;
} Header;

void heap_debug_dump(
  const char *operation,
  char *heap_start,
  char *heap_end,
  Header *bins[NUMBINS]
);

#endif
