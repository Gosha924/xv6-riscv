#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define NSIZES 7

typedef struct header {
  struct header *next;
  uint size;
} Header;

uint sizes[NSIZES] = {32, 64, 128, 256, 512, 1024, 2048};
Header* freeList[NSIZES];
Header* largeList;

void* malloc(uint64 nbytes) {
    uint class = 0;
    while(sizes[class] < nbytes) {
      class++;
    }

    if (class < 7) {
      Header* h = freeList[class];
      if (h == NULL) {
        if (largeList != NULL) {
          /*списать память с большого списка*/
        } else {
          /*сис. вызов и нарезка на этот класс*/
        }
      }
      freeList[class] = h->next;
      return (void*)(h + 1);
    } else {
      if (largeList == NULL) {
        /*сис. вызов и выдать всё пользователю*/
      } else {
        /*списать память с большого списка*/
      }
    }
}