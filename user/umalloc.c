#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define NSIZES 7
#define PGSIZE 4096
#define ALLOC_SIZE (2 * PGSIZE)

typedef struct header {
  struct header *next;
  uint size;
} Header;

static uint sizes[NSIZES] = {32, 64, 128, 256, 512, 1024, 2048};
static Header* freeList[NSIZES];
static Header* largeList;

static Header *take_mem(uint min) {
  for (Header **p = &largeList; *p; p = &(*p)->next)
    if ((*p)->size >= min) {
      Header *h = *p;
      *p = h->next;
      return h;
    }
  return 0;
}

static void slicing(Header *mem, uint total, uint class) {
  uint step  = sizes[class] + sizeof(Header);
  uint n = total / step;
  char* base = (char*)mem;

  for (uint i = 0; i < n; i++) {
    Header* h = (Header*)(base + i * step);
    h->size = sizes[class];
    h->next = freeList[class];
    freeList[class] = h;
  }

  uint tail = total - n * step;

  if (tail > sizeof(Header) + sizes[NSIZES - 1]) {
    Header *t = (Header *)(base + n * step);
    t->size = tail - sizeof(Header);
    t->next = largeList;
    largeList = t;
  }
}

void* malloc(uint nbytes) {
    uint class = 0;
    while(class < NSIZES && sizes[class] < nbytes) {
      class++;
    }

    // Маленькие аллокации

    if (class < NSIZES) {
      if (freeList[class] == NULL) {

        Header* areaOfMem = take_mem(sizes[class]);

        if (areaOfMem != NULL) {
          uint totalSize = areaOfMem->size + sizeof(Header);
          slicing(areaOfMem, totalSize, class);
        } else {
          char *mem = sbrk(ALLOC_SIZE);
          if (mem == (char*)-1) return 0;

          slicing((Header *)mem, ALLOC_SIZE, class);
        }
      }

      Header* h = freeList[class];
      freeList[class] = h->next;
      return (void*)(h + 1);
    }

    // Большие аллокации

    Header *h = take_mem(nbytes);
    if (h != NULL) return (void *)(h + 1);

    uint need = nbytes + sizeof(Header);
    uint pages = (need + PGSIZE - 1) / PGSIZE;

    char* mem = sbrk(PGSIZE * pages);
    if (mem == (char*)-1) return 0;

    Header* h = (Header*)mem;
    h->size = PGSIZE * pages - sizeof(Header);
    return (void*)(h + 1);
}

void free(void* pointer) {
  if (pointer == NULL) return;

  Header* h = (Header*)pointer - 1;
  
  uint class = 0;
  while(class < NSIZES && sizes[class] < h->size) {
      class++;
  }

  if (class < NSIZES) {
    h->next = freeList[class];
    freeList[class] = h;
    
  } else {
  h->next = largeList;
  largeList = h;
  }
}