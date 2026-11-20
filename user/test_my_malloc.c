#include "kernel/types.h"
#include "user/user.h"

typedef struct Node {
  int value;
  struct Node *next;
} Node;

int
main(void){
  Node *head = 0;
  Node *node;

  for(int i = 0; i < 3; i++){
    node = malloc(sizeof(Node));
    if(node == 0){
      printf("malloc failed\n");
      exit(1);
    }

    node->value = i;
    node->next = head;
    head = node;
  }

  printf("List:");
  for(node = head; node != 0; node = node->next){
    printf(" %d", node->value);
  }
  printf("\n");

  while(head != 0){
    node = head;
    head = head->next;
    free(node);
  }

  printf("All nodes freed\n");
  exit(0);
}
