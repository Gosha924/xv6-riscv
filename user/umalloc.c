#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define SIZEHEADER sizeof(block_t)
#define NUMBINS 9
#define PAGESIZE 4096
#define MINSPLIT (sizeof(block_t) + 8)

typedef struct block {
  uint64 size;
  uint64 is_free; // 0  = занят, 1  = свободен
  struct block* next;
  struct block* next_free;
} block_t;


block_t *bins[NUMBINS];
block_t *firstBlock;
block_t *lastBlock;
int heap_ready;

static int get_bin(uint64 size) {
  if (size < 16) return 0;
  if (size < 32) return 1;
  if (size < 64) return 2;
  if (size < 128) return 3;
  if (size < 256) return 4;
  if (size < 512) return 5;
  if (size < 1024) return 6;
  if (size < 2048) return 7;
  return 8;
}

static void heap_init(void) {
  if (heap_ready) return;
  for (int i = 0; i < NUMBINS; i++) {
    bins[i] = 0;
  }
  firstBlock = 0;
  lastBlock = 0;
  heap_ready = 1;
}


static block_t* extend_heap(uint64 needSizeMemory) {
  int pages = (needSizeMemory + PAGESIZE - 1) / PAGESIZE;
  if (pages == 0) {
    pages = 1;
  }
  int alloc = pages * PAGESIZE;

  char *mem = sbrk(alloc);
  if (mem == (char*) -1) {
    return 0;
  }
  block_t *new_block = (block_t *)mem;
  new_block->size = alloc;
  new_block->is_free = 1;
  new_block->next = 0;
  new_block->next_free = 0;

  if (lastBlock != 0) {
    lastBlock->next = new_block;
  } else {
    firstBlock = new_block;
  }
  lastBlock = new_block;

  // в массиве храняться первые блоки соотв размера, а остальные можно получить через next_free
  int indexInBin = alloc;
  new_block->next_free = bins[indexInBin];
  bins[indexInBin] = new_block;
  return new_block;
}

static void remove_block_from_bin(block_t *block) {
  int index = get_bin(block->size);
  block_t *cur = bins[index];
  block_t *prev = 0;

  while (cur != 0) {
    if (cur == block) {
      if (prev != 0) {
        prev->next_free = cur->next_free;
      } else {
        bins[index] = cur->next_free;
      }
      block->next_free = 0;
      return;
    }
    prev = cur;
    cur = cur->next_free;
  }
}

static void split_block(block_t *block, uint64 need) {
  if (block->size < need + MINSPLIT) {
    return;
  }
  char *block_addr = (char *)block;
  char *rest_addr = block_addr + need;

  block_t *rest = (block_t *)rest_addr;
  rest->size = block->size - need;
  rest->is_free = 1;
  rest->next = block->next;
  rest->next_free = 0;

  block->size = need;
  block->next = rest;

  if (lastBlock == block) {
    lastBlock = rest;
  }
  int index = get_bin(rest->size);
  rest->next_free = bins[index];
  bins[index] = rest;
}

void* malloc(uint userSize) {
  if (!heap_ready) heap_init();
  if (userSize == 0) return 0;

  uint64 totalSize = userSize + SIZEHEADER;
  totalSize = (totalSize + 7) & ~7;
  int startBin = get_bin(totalSize);

  block_t *found = 0;
  for (int i = startBin; i < NUMBINS; i++) {
    block_t *block = bins[i];
    while (block != 0) {
      if (block->size >= totalSize) {
        found = block;
        break;
      }
      block = block->next_free;
    }
    if (found != 0) break;
  }

  if (found == 0) {
    found = extend_heap(totalSize);
    if (found == 0) return 0;
  }

  remove_block_from_bin(found);
  split_block(found, totalSize);
  found->is_free = 0;

  char *found_addr = (char *)found;
  char *data_addr = found_addr + SIZEHEADER;
  return (void *)data_addr;
}


void free(void *ptr) {
    (void)ptr;
}