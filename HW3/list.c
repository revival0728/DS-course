// Data Structure HW3
// Example Input:
//  5
//  1 2 3 4 5
//  2
// Example Output:
//  Input list: [1, 2, 3, 4, 5]
//  Target number node at 0x609d481f9340

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void* malloc_s(size_t size) {
  void *m = malloc(size);
  if(m == NULL) {
    printf("Memory Allocation Failed.");
    return NULL;
  }
  return m;
}

#define ELE_T int
typedef struct list_t {
  ELE_T val;
  struct list_t *next, *prev;
} list_t, *list;

#define ALLOC_CNT 1
#define ALLOC_SIZE (sizeof(list_t) * ALLOC_CNT)
typedef struct allocator_t {
  list_t *top, *end;
  void* mem;
  struct allocator_t *next, *prev;
} allocator_t, *allocator;

allocator dalloc;

allocator allocator_new() {
  allocator nalloc = malloc_s(sizeof(allocator_t));
  nalloc->mem = malloc_s(ALLOC_SIZE);
  nalloc->top = nalloc->mem;
  nalloc->end = nalloc->top + ALLOC_CNT;
  nalloc->next = NULL;
  nalloc->prev = NULL;
  return nalloc;
}

list dalloc_alloc() {
  assert(dalloc != NULL);
  assert(dalloc->top != dalloc->end);
  list ret = dalloc->top++;
  if(dalloc->top == dalloc->end) {
    dalloc->next = allocator_new();
    dalloc->next->prev = dalloc;
    dalloc = dalloc->next;
  }
  return ret;
}

void dalloc_free_self() {
  assert(dalloc != NULL);
  allocator front = dalloc;
  while(front->prev != NULL) {
    front = front->prev;
  }
  while(front->next != NULL) {
    allocator next = front->next;
    free(front);
    front = next;
  }
}

list list_create(ELE_T val) {
  list lst = dalloc_alloc();
  lst->val = val;
  lst->next = NULL;
  lst->prev = NULL;
  return lst;
}

// Add another list to list "lst"
// @return last element of new lst (assume that lst is the list tail)
list list_append(list lst, list other) {
  if(lst == NULL && other == NULL) return NULL;
  if(lst == NULL) return other;
  if(other == NULL) return lst;
  list ret = NULL;
  if(other->prev && lst->next) {
    // len(other) > 1 && len(lst) > 1
    ret = other->prev;
    other->prev->next = lst->next;
    lst->next = other;
    other->prev = lst;
    lst->next->prev = other->prev;
  } else if(lst->next) {
    // len(other) == 1 && len(lst) > 1
    ret = other;
    other->next = lst->next;
    other->prev = lst;
    lst->next = other;
    lst->next->prev = other;
  } else if(other->prev) {
    // len(other) > 1 && len(lst) == 1
    ret = other->prev;
    lst->next = other;
    lst->prev = other->prev;
    other->prev->next = lst;
    other->prev = lst;
  } else {
    // len(other) == 1 && len(lst) == 1
    ret = other;
    lst->next = other;
    lst->prev = other;
    other->next = lst;
    other->prev = lst;
  }
  return ret;
}

list list_search(list lst, ELE_T val) {
  if(lst == NULL) return NULL;
  if(lst->val == val) return lst;
  list ret = NULL;
  for(list cur = lst->next, head = lst; cur != head; cur = cur->next) {
    if(cur->val == val) {
      return cur;
    }
  }
  return NULL;
}

list list_read() {
  int n;
  printf("Please input length of the list: ");
  scanf("%d", &n);
  printf("Please input list: ");
  list retp = NULL;
  for(int t, i = 0; i < n; ++i) {
    scanf("%d", &t);
    retp = list_append(retp, list_create(t));
  }
  return retp->next;
}

void list_println(list lst) {
  if(lst == NULL) return;
  printf("[%d", lst->val);
  for(list cur = lst->next, head = lst; cur != head; cur = cur->next) {
    printf(", %d", cur->val);
  }
  printf("]\n");
}

int main() {
  dalloc = allocator_new();
  list lst = list_read();
  int tnum;
  printf("Please input target number: ");
  scanf("%d", &tnum);
  list tar = list_search(lst, tnum);
  printf("Input list: ");
  list_println(lst);
  if(tar)
    printf("Target number node at %p", tar);
  else
    printf("Target number not found.");
  dalloc_free_self();
}
