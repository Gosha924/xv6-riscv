#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/umalloc.h"

void heap_debug_dump(
  const char *operation,
  char *heap_start,
  char *heap_end,
  Header *bins[NUMBINS]
){
  uint64 used = 0;
  uint64 free_bytes = 0;
  uint64 allocated;

  printf("\n========== HEAP: %s ==========\n", operation);

  if (heap_start == 0 || heap_end == 0 || heap_end <= heap_start) {
    printf("Heap is empty or boundaries are invalid.\n");
    printf("================================\n");
    return;
  }

  allocated = (uint64)(heap_end - heap_start);

  printf("Heap start: %p\n", heap_start);
  printf("Heap end:   %p\n", heap_end);
  printf("Allocated heap: %ld bytes\n", allocated);

  printf("\nAll heap blocks (physical order):\n");

  Header *block = (Header *)heap_start;

  while ((char *)block < heap_end) {
    if (block->size < SIZEHEADER ||
        block->size > (uint64)(heap_end - (char *)block)) {
      printf("Invalid block header at %p (size=%ld)\n", block, block->size);
      break;
    }

    printf("  %p | size=%ld | %s\n", block, block->size, block->is_free ? "FREE" : "USED");

    if (block->is_free)
      free_bytes += block->size - SIZEHEADER;
    else
      used += block->size - SIZEHEADER;

    block = (Header *)((char *)block + block->size);
  }

  if ((char *)block != heap_end)
    printf("Physical blocks do not cover the entire heap\n");

  printf("\nSegregated Free Lists:\n");

  for (int i = 0; i < NUMBINS; i++) {
    printf("  bin[%d]:", i);

    block = bins[i];

    int count = 0;
    while (block != 0 && count < allocated / SIZEHEADER) {
      printf(" [%p, size=%ld]", block, block->size);
      block = block->next_free;
      count++;
    }

    if (block != 0)
      printf("Possible cycle in bin[%d]", i);

    printf("\n");
  }

  printf("\nPayload used: %ld bytes\n", used);
  printf("Payload free: %ld bytes\n", free_bytes);

  if (allocated != 0)
    printf("Heap utilization: %ld%%\n", used * 100 / allocated);

  printf("================================\n");
}
