#ifndef USER_UMALLOC_H
#define USER_UMALLOC_H

#include "kernel/types.h"

#define NUMBINS 9

typedef struct block {
  uint64 size;
  uint64 is_free; // 0 = занят, 1 = свободен
  struct block* next_free;
} block_t;

void heap_debug_dump(
  const char *operation,
  char *heap_start,
  char *heap_end,
  block_t *bins[NUMBINS]
);

#endif
