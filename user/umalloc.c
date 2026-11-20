#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define SIZEHEADER sizeof(Header)
#define NUMBINS 8
#define PAGESIZE 4096
#define MINSPLIT (SIZEHEADER + 8)

typedef struct header {
  uint64 size;
  uint64 is_free;
  struct header* next_free;
} Header;

static Header *bins[NUMBINS];

static int get_bin(uint64 size) {
  if (size < 32) return 0;
  if (size < 64) return 1;
  if (size < 128) return 2;
  if (size < 256) return 3;
  if (size < 512) return 4;
  if (size < 1024) return 5;
  if (size < 2048) return 6;
  return 7;
}

static Header* extend_heap(uint64 needSizeMemory) {
  uint64 pages = (needSizeMemory + PAGESIZE - 1) / PAGESIZE;
  if (pages == 0) {
    pages = 1;
  }
  uint64 alloc = pages * PAGESIZE;
  printf("EXTEND: Requesting %ld bytes from kernel (pages=%ld, alloc=%ld)\n", needSizeMemory, pages, alloc);

  char *mem = sbrk(alloc);
  if (mem == (char*) -1) {
    printf("EXTEND: ERROR sbrk(%ld) failed\n", alloc);
    return 0;
  }
  Header *new_block = (Header *)mem;
  new_block->size = alloc;
  new_block->is_free = 1;
  new_block->next_free = 0;

  // в массиве храняться первые блоки соотв размера, а остальные можно получить через next_free
  int indexInBin = get_bin(alloc);
  new_block->next_free = bins[indexInBin];
  bins[indexInBin] = new_block;
  printf("EXTEND: Created block at %p, size=%ld, added to bin[%d]\n",
           new_block, alloc, indexInBin);
  return new_block;
}

static void remove_block_from_bin(Header *block) {
  int index = get_bin(block->size);
  Header *cur = bins[index];
  Header *prev = 0;

  while (cur != 0) {
    if (cur == block) {
      if (prev != 0) {
        prev->next_free = cur->next_free;
        printf("REMOVE: Removed block %p from middle of bin[%d]\n", block, index);
      } else {
        bins[index] = cur->next_free;
        printf("REMOVE: Removed block %p from head of bin[%d]\n", block, index);
      }
      block->next_free = 0;
      return;
    }
    prev = cur;
    cur = cur->next_free;
  }
  printf("REMOVE: ERROR block %p not found in bin[%d]\n", block, index);
}

static void split_block(Header *block, uint64 need) {
  if (block->size < need + MINSPLIT) {
    printf("SPLIT: Header %p (size=%ld) too small for split (need=%ld, MINSPLIT=%ld)\n", block, block->size, need, MINSPLIT);
    return;
  }
  char *block_addr = (char *)block;
  char *rest_addr = block_addr + need;

  Header *rest = (Header *)rest_addr;
  rest->size = block->size - need;
  rest->is_free = 1;
  rest->next_free = 0;

  block->size = need;

  int index = get_bin(rest->size);
  rest->next_free = bins[index];
  bins[index] = rest;
  printf("SPLIT: Block %p split into %ld + %ld (remainder at %p, bin[%d])\n", block, need, rest->size, rest, index);
}

void* malloc(uint userSize) {
  printf("\nMALLOC: Request for %d bytes\n", userSize);
  if (userSize == 0) return 0;

  uint64 totalSize = userSize + SIZEHEADER;
  if (totalSize % 8 != 0) {
    totalSize = totalSize + (8 - (totalSize % 8));
  }
  printf("MALLOC: totalSize (with header, aligned) = %ld\n", totalSize);
  int startBin = get_bin(totalSize);

  Header *found = 0;
  for (int i = startBin; i < NUMBINS; i++) {
    Header *block = bins[i];
    while (block != 0) {
      if (block->size >= totalSize) {
        found = block;
        printf("MALLOC: Found block %p in bin[%d] (size=%ld)\n", found, i, block->size);
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
  printf("MALLOC: SUCCESS returning %p (block=%p, size=%ld)\n", data_addr, found, found->size);
  return (void *)data_addr;
}

void free(void *ptr) {
    printf("\nFREE: Request to free %p\n", ptr);
    if (ptr == 0) return;
    char* header_addr = (char *)ptr - SIZEHEADER;
    Header *block = (Header *)header_addr;
    printf("FREE: Block header at %p, size=%ld\n", block, block->size);

    if (block->is_free) {
      printf("FREE: ERROR double free at %p\n", ptr);
      return;
    }
    block->is_free = 1;

    int index = get_bin(block->size);
    block->next_free = bins[index];
    bins[index] = block;
    printf("FREE: SUCCESS block %p returned to bin[%d]\n", block, index);
    return;
}