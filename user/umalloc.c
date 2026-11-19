#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define NSIZES 7
#define PGSIZE 4096

typedef struct header {
  struct header *next;
  uint size;
} Header;

static uint sizes[NSIZES] = {32, 64, 128, 256, 512, 1024, 2048};
static Header* freeList[NSIZES];
static Header* largeList;

void* malloc(uint nbytes) {
    uint class = 0;
    while(class < NSIZES && sizes[class] < nbytes) {  //нашли класс
      class++;
    }

    if (class < NSIZES) {
      if (freeList[class] == NULL) { //Если соответствующий список пуст
        if (largeList != NULL) {
          /*списать память(страницу найти) с большого списка*/
        } else {
          /*сис. вызов и нарезка на этот класс*/
        }
      }

      Header* h = freeList[class];
      freeList[class] = h->next;
      return (void*)(h + 1);
    } else {   //Если вышли за границы sizes[]
      /*найти в большом списке страницу и выдать пользователю*/
      



      /*Если страницу не нашли - сис. вызов и выдать всё пользователю*/
      need = nbytes + sizeof(Header);
      int pages = (need + PGSIZE - 1) / PGSIZE;

      char* mem = sbrk(PGSIZE * pages);
      if (mem == SBRK_ERROR) return 0;

      Header* h = (Header*)mem;
      h->size = PGSIZE * pages;
      return (void*)(h + 1);
    }
}