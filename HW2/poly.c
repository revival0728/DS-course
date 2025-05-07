#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

void* malloc_s(size_t size) {
  void *p = malloc(size);
  if(p == NULL) {
    printf("memory allocation failed.");
    return NULL;
  }
  return p;
}


// store polynomial in increasing order
#define COEFF_T int
#define DEG_T int
#define DEG_MIN (1 << 31)
typedef struct poly_t {
  COEFF_T coeff;
  DEG_T deg;
  struct poly_t *next;
} poly_t, *poly;

#define STK_ISZ 100
#define STK_ELE_T poly
#define STK_EMPTY_E NULL
typedef struct stack_t {
  int top, cur;
  STK_ELE_T stk[STK_ISZ];
  struct stack_t *next;
} stack_t, *stack;

stack stk_create() {
  stack s = malloc_s(sizeof(stack_t));
  s->top = -1;
  s->cur = 1;
  s->next = NULL;
  return s;
}

void stk_free(stack s) {
  if(s == NULL) return;
  stack next = s->next;
  while(s != NULL) {
    next = s->next;
    free(s);
    s = next;
  }
}

void stk_push(stack s, STK_ELE_T val) {
  if(s == NULL) return;
  while(!s->cur) {
    s = s->next;
    assert(s != NULL);
  };
  if(s->top + 1 == STK_ISZ) {
    s->next = stk_create();
    s->cur = 0;
    s = s->next;
  }
  s->stk[++s->top] = val;
}

STK_ELE_T stk_pop(stack s) {
  if(s == NULL) return STK_EMPTY_E;
  stack prev = NULL;
  while(!s->cur) {
    prev = s;
    s = s->next;
    assert(s != NULL);
  }
  if(s->top == -1) return STK_EMPTY_E;
  STK_ELE_T ret = s->stk[s->top--];
  if(s->top == -1 && prev != NULL) {
    s->cur = 0;
    prev->cur = 1;
  }
  return ret;
}

poly poly_create(COEFF_T coeff, DEG_T deg) {
  poly p = malloc_s(sizeof(poly_t));
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
    free(p);
    p = next;
  }
}

// Append another poly pp to poly p
// return pp
poly poly_append(poly p, poly pp) {
  fprintf(stderr, "poly_append()\n");
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
      free(f);
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
  fprintf(stderr, "pmult()\n");
  if(p1 == NULL || p2 == NULL) return NULL;
  poly ret = NULL, pp1 = p1;
  while(pp1 != NULL) {
    poly res = NULL,  resp = NULL, pp2 = p2;
    res = poly_create(0, DEG_MIN);
    resp = res;
    fprintf(stderr, "pmult(): multiply\n");
    while(pp2 != NULL) {
      DEG_T deg = pp1->deg + pp2->deg;
      COEFF_T coeff = pp1->coeff * pp2->coeff;
      resp = poly_append(resp, poly_create(coeff, deg));
      pp2 = pp2->next;
    }
    fprintf(stderr, "pmult(): merge\n");
    ret = poly_merge(ret, res->next);
    pp1 = pp1->next;
  }
  return ret;
}

// Create new polynomial from user input
// input format example: 2 1 3 4 0 -> 3x^4 + 2x^1
poly poly_create_from_input() {
  COEFF_T coeff;
  DEG_T deg, cur_mx_deg = DEG_MIN;
  poly ret = NULL, retp = NULL;
  ret = poly_create(0, DEG_MIN);
  retp = ret;
  scanf("%d", &coeff);
  while(coeff != 0) {
    scanf("%d", &deg);
    fprintf(stderr, "coeff, deg = %d, %d\n", coeff, deg);
    if(ret != NULL && cur_mx_deg >= deg) {
      printf("Please input elements in increasing order");
      scanf("%d", &coeff);
      continue;
    }
    cur_mx_deg = cur_mx_deg < deg ? deg : cur_mx_deg;
    fprintf(stderr, "input()\n");
    retp = poly_append(retp, poly_create(coeff, deg));
    assert(ret != NULL);
    scanf("%d", &coeff);
  }
  return ret->next;
}

stack p_stk = NULL;
// Print polynomial in descending order
void poly_println(poly p) {
  int head = 1;
  while(p != NULL) {
    stk_push(p_stk, p);
    p = p->next;
  }
  while(p = stk_pop(p_stk)) {
    int is_constant = p->deg == 0;
    if(head) {
      if(p->coeff != 1 && p->coeff != -1)
        printf("%d", p->coeff);
    } else {
      if(p->coeff > 0) {
        if(p->coeff > 1 || (p->coeff == 1 && is_constant))
          printf(" + %d", p->coeff);
      } else {
        if(p->coeff < -1 || (p->coeff == -1 && is_constant))
          printf(" - %d", -p->coeff);
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
  while(p != NULL) {
    fprintf(stderr, "poly_eprintln: coeff, deg = %d, %d\n", p->coeff, p->deg);
    p = p->next;
  }
}

int main() {
  p_stk = stk_create();
  poly p1 = NULL, p2 = NULL, pm = NULL;
  printf("Input polynomial1 [(coeff)(deg), ..., 0]: ");
  p1 = poly_create_from_input();
  poly_eprintln(p1);
  printf("Input polynomial2 [(coeff)(deg), ..., 0]: ");
  p2 = poly_create_from_input();
  poly_eprintln(p2);
  pm = pmult(p1, p2);
  printf("polynomial 1: "); poly_println(p1);
  printf("polynomial 2: "); poly_println(p2);
  printf("multiplication result: "); poly_println(pm);

  stk_free(p_stk);
  poly_free(p1);
  poly_free(p2);
  poly_free(pm);
}