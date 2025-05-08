// Data Structrue HW2
// Example Input:
//  1 4 2 1 0
//  1 5 2 1 0
// Example Output:
//  polynomial 1: x^4 + 2x^1
//  polynomial 2: x^5 + 2x^1
//  multiplication result: x^9 + 2x^6 + 2x^5 + 4x^2
// pmult() is at line 272.
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

// Add DEBUG to see function logs
// #define DEBUG

#ifdef DEBUG
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#endif
#ifndef DEBUG
#define eprintf(format, ...)
#endif

#define COEFF_T int
#define DEG_T int
#define DEG_MIN (1 << 31)
#define DEG_MAX (1 << 30)
// Polynomial struct
// Store polynomial in increasing order
// Time Complexity:
// create: O(1)
// free  : O(N)
// append: O(1)
// merge : O(N+M)
// pmult : O(NM)
typedef struct poly_t {
  COEFF_T coeff;
  DEG_T deg;
  struct poly_t *next;
} poly_t, *poly;

#define STK_ISZ 100
#define STK_ELE_T poly
#define STK_EMPTY_E NULL
// Stack, a LIFO container
// Store "poly"
// create: O(1)
// free  : O(N)
// push  : O(1)
// pop   : O(1)
typedef struct stack_t {
  int top, cur;
  STK_ELE_T stk[STK_ISZ];
  struct stack_t *next, *prev;
} stack_t, *stack;

#define UALLOC_SZ (sizeof(poly_t))
#define PALLOC_CNT 100
#define PALLOC_SZ (UALLOC_SZ * PALLOC_CNT)
// Poly Allocator Type
// Create to fix memeory fragmentation problem and prevent from memory leaking
// Time Complexity:
// create   : O(PALLOC_SZ) -> O(1)
// alloc    : O(1)
// free     : O(1)
// free_self: O(PALLOC_SZ) -> O(1)
typedef struct palloc_t {
  int full, cmem;
  void *mem;
  stack rc_stk;
  struct palloc_t *next, *prev;
} palloc_t, *palloc;


// default global variables or functions
palloc pallocator;  // poly allocator
stack p_stk;        // poly stack     : for reversing a polynomial without changing origin
#define CHECK_P_STK() assert(stk_empty(p_stk))

void* malloc_s(size_t size) {
  void *p = malloc(size);
  if(p == NULL) {
    printf("memory allocation failed.\n");
    return NULL;
  }
  return p;
}

stack stk_create() {
  stack s = malloc_s(sizeof(stack_t));
  s->top = -1;
  s->cur = 1;
  s->next = NULL;
  s->prev = NULL;
  return s;
}

void stk_free(stack s) {
  if(s == NULL) return;
  while(s->prev != NULL) s = s->prev;
  stack next = s->next;
  while(s != NULL) {
    next = s->next;
    free(s);
    s = next;
  }
}

void stk_push(stack *sp, STK_ELE_T val) {
  assert(sp != NULL);
  stack s = *sp;
  if(s == NULL) return;
  assert(s->cur == 1);
  if(s->top + 1 == STK_ISZ) {
    if(s->next == NULL) {
      s->next = stk_create();
      s->next->prev = s;
    } else {
      s->next->cur = 1;
    }
    s->cur = 0;
    s = s->next;
    *sp = s;
  }
  s->stk[++s->top] = val;
}

STK_ELE_T stk_pop(stack *sp) {
  assert(sp != NULL);
  stack s = *sp;
  if(s == NULL) return STK_EMPTY_E;
  assert(s->cur == 1);
  if(s->top == -1) return STK_EMPTY_E;
  STK_ELE_T ret = s->stk[s->top--];
  if(s->top == -1 && s->prev != NULL) {
    s->cur = 0;
    s->prev->cur = 1;
    *sp = s->prev;
  }
  return ret;
}

int stk_empty(stack s) {
  if(s == NULL) return 1;
  return s->top == -1 && s->prev == NULL;
}

palloc palloc_create() {
  palloc npac = malloc_s(sizeof(palloc_t));
  npac->full = 0;
  npac->cmem = 0;
  npac->mem = malloc_s(PALLOC_SZ);
  npac->next = NULL;
  npac->prev = NULL;
  return npac;
}

palloc palloc_create_global() {
  palloc npac = palloc_create();
  npac->rc_stk = stk_create();
  return npac;
}

poly palloc_alloc() {
  eprintf("palloc_alloc()\n");
  palloc pac = pallocator;
  assert(pac != NULL);
  assert(pac->full == 0);
  poly ret = stk_pop(&pac->rc_stk);
  if(ret) {
    eprintf("begin_mem: %p, alloc_mem: %p\n", (poly)(pac->mem), ret);
    return ret;
  }
  assert(pac->cmem < PALLOC_CNT);
  ret = (poly)(pac->mem) + pac->cmem++;
  if(pac->cmem == PALLOC_CNT && stk_empty(pac->rc_stk)) {
    pac->full = 1;
    pac->next = palloc_create();
    pac->next->prev = pac;
    pac->next->rc_stk = pac->rc_stk;
    pallocator = pac->next;
  }
  eprintf("begin_mem: %p, alloc_mem: %p\n", (poly)(pac->mem), ret);
  return ret;
}

void palloc_free(poly *p) {
  palloc pac = pallocator;
  assert(pac != NULL);
  if(p == NULL) return;
  if(*p == NULL) return;
  pac->full = 0;
  stk_push(&pac->rc_stk, *p);
  *p = NULL;
}

void palloc_free_self() {
  palloc pac = pallocator;
  assert(pac != NULL);
  while(pac->prev != NULL) pac = pac->prev;
  stk_free(pac->rc_stk);
  palloc next = pac->next;
  while(pac) {
    next = pac->next;
    free(pac->mem);
    free(pac);
    pac = next;
  }
}

poly poly_create(COEFF_T coeff, DEG_T deg) {
  poly p = palloc_alloc();
  p->coeff = coeff;
  p->deg = deg;
  p->next = NULL;
  return p;
}

void poly_free(poly p) {
  if(p == NULL) return;
  poly next = p->next;
  while(p != NULL) {
    next = p->next;
    palloc_free(&p);
    p = next;
  }
}

// Append another poly pp to poly p
// Check if the append operation is legal
// @return pp
poly poly_append(poly p, poly pp) {
  eprintf("poly_append()\n");
  if(p == NULL) return pp;
  if(pp == NULL) return p;
  assert(p->next == NULL);
  assert(p->deg < pp->deg);
  p->next = pp;
  return pp;
}

#define poly_merge_move_ptr(ptr) tp=ptr->next;ptr->next=NULL;ptr=tp;
// Add two polynomial without new memory allocation
// Notice that it will break p1, p2
// If p1, p2 have same degree element, merge to p1
poly poly_merge(poly p1, poly p2) {
  eprintf("poly_merge()\n");
  if(p1 == NULL) return p2;
  if(p2 == NULL) return p1;
  poly pp1 = p1, pp2 = p2, ret = NULL, retp = NULL, tp = NULL;
  ret = poly_create(0, DEG_MIN);
  retp = ret;
  while(pp1 != NULL && pp2 != NULL) {
    if(pp1->deg == pp2->deg) {
      pp1->coeff += pp2->coeff;
      if(pp1->coeff != 0)
        retp = poly_append(retp, pp1);
      poly f = pp2;
      poly_merge_move_ptr(pp1);
      poly_merge_move_ptr(pp2);
      palloc_free(&f);
    } else if(pp1->deg > pp2->deg) {
      retp = poly_append(retp, pp2);
      poly_merge_move_ptr(pp2);
    } else {
      retp = poly_append(retp, pp1);
      poly_merge_move_ptr(pp1);
    }
  }
  if(pp1 != NULL) retp = poly_append(retp, pp1);
  if(pp2 != NULL) retp = poly_append(retp, pp2);
  return ret->next;
}

poly pmult(poly p1, poly p2) {
  eprintf("pmult()\n");
  if(p1 == NULL || p2 == NULL) return NULL;
  poly ret = NULL, pp1 = p1;
  for(; pp1; pp1 = pp1->next) {
    poly res = NULL,  resp = NULL, pp2 = p2;
    res = poly_create(0, DEG_MIN);
    resp = res;
    eprintf("pmult(): multiply\n");
    for(; pp2; pp2 = pp2->next) {
      DEG_T deg = pp1->deg + pp2->deg;
      COEFF_T coeff = pp1->coeff * pp2->coeff;
      resp = poly_append(resp, poly_create(coeff, deg));
    }
    eprintf("pmult(): merge\n");
    ret = poly_merge(ret, res->next);
  }
  return ret;
}

// Create new polynomial from user input
// input format example: 2 1 3 4 0 -> 3x^4 + 2x^1
poly poly_create_from_input() {
  CHECK_P_STK();
  COEFF_T coeff;
  DEG_T deg, cur_mn_deg = DEG_MAX;
  poly ret = NULL, retp = NULL;
  ret = poly_create(0, DEG_MIN);
  retp = ret;
  scanf("%d", &coeff);
  while(coeff != 0) {
    scanf("%d", &deg);
    eprintf("Input coeff, deg = %d, %d\n", coeff, deg);
    if(ret != NULL && cur_mn_deg <= deg) {
      printf("Please input elements in descending order\n");
      scanf("%d", &coeff);
      continue;
    }
    cur_mn_deg = cur_mn_deg > deg ? deg : cur_mn_deg;
    stk_push(&p_stk, poly_create(coeff, deg));
    assert(ret != NULL);
    scanf("%d", &coeff);
  }
  poly top = NULL;
  while(top = stk_pop(&p_stk)) {
    retp = poly_append(retp, top);
  }
  return ret->next;
}

// Print polynomial in descending order
void poly_println(poly p) {
  CHECK_P_STK();
  int head = 1;
  while(p != NULL) {
    stk_push(&p_stk, p);
    p = p->next;
  }
  while(p = stk_pop(&p_stk)) {
    int is_constant = p->deg == 0;
    if(head) {
      if(p->coeff != 1 && p->coeff != -1)
        printf("%d", p->coeff);
    } else {
      if(p->coeff > 0) {
        if(p->coeff > 1 || (p->coeff == 1 && is_constant))
          printf(" + %d", p->coeff);
        else
          printf(" + ");
      } else {
        if(p->coeff < -1 || (p->coeff == -1 && is_constant))
          printf(" - %d", -p->coeff);
        else
          printf(" - ");
      }
    }
    if(!is_constant) {
      printf("x^%d", p->deg);
    }
    head = 0;
    p = p->next;
  }
  printf("\n");
}

// Print polynomial for debug
void poly_eprintln(poly p) {
  #ifdef DEBUG
  while(p != NULL) {
    eprintf("poly_eprintln: coeff, deg = %d, %d\n", p->coeff, p->deg);
    p = p->next;
  }
  #endif
}

int main() {
  pallocator = palloc_create_global();
  p_stk = stk_create();

  poly p1 = NULL, p2 = NULL, pm = NULL;
  printf("Input polynomial1 [(coeff)(deg), ..., 0]: ");
  p1 = poly_create_from_input();
  poly_eprintln(p1);
  printf("Input polynomial2 [(coeff)(deg), ..., 0]: ");
  p2 = poly_create_from_input();
  poly_eprintln(p2);
  pm = pmult(p1, p2);
  printf("\n");
  printf("polynomial 1: "); poly_println(p1);
  printf("polynomial 2: "); poly_println(p2);
  printf("multiplication result: "); poly_println(pm);

  stk_free(p_stk);
  poly_free(p1);
  poly_free(p2);
  poly_free(pm);
  palloc_free_self();
}