#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#define COEFF_T int
#define DEG_T int

// store polynomial in increasing order
typedef struct poly {
  COEFF_T coeff;
  DEG_T deg;
  struct poly *next;
} poly_t, *poly;

poly poly_create(COEFF_T coeff, DEG_T deg) {
  poly p = malloc(sizeof(poly_t));
  if (p == NULL) {
    printf("aaa");
    return NULL;
  };
  p->coeff = coeff;
  p->deg = deg;
  p->next = NULL;
  return p;
}

poly poly_append(poly p, poly pp) {
  if(p == NULL) return pp;
  if(pp == NULL) return p;
  poly cur = p;
  while (cur->next != NULL) {
    assert(cur->deg < pp->deg);
    cur = cur->next;
  }
  cur->next = pp;
  return p;
}

// Add two polynomial without new memory allocation
// If p1, p2 have same degree element, merge to p1
poly poly_addeq(poly p1, poly p2) {
  if(p1 == NULL) return p2;
  if(p2 == NULL) return p1;
  poly pp1 = p1, pp2 = p2, ret = NULL;
  while(pp1 != NULL && pp2 != NULL) {
    if(pp1->deg == pp2->deg) {
      pp1->coeff += pp2->coeff;
      if(pp1->coeff != 0)
        ret = poly_append(ret, pp1);
      pp1 = pp1->next;
      pp2 = pp2->next;
    } else if(pp1->deg > pp2->deg) {
      ret = poly_append(ret, pp2);
      pp2 = pp2->next;
    } else {
      ret = poly_append(ret, pp1);
      pp1 = pp1->next;
    }
  }
  while(pp1 != NULL) poly_append(ret, pp1);
  while(pp2 != NULL) poly_append(ret, pp2);
  return ret;
}

poly pmult(poly p1, poly p2) {
  if(p1 == NULL || p2 == NULL) return NULL;
  poly ret = NULL, pp1 = p1;
  while(pp1 != NULL) {
    poly res = NULL, pp2 = p2;
    while(pp2 != NULL) {
      DEG_T deg = pp1->deg + pp2->deg;
      COEFF_T coeff = pp1->coeff * pp2->coeff;
      res = poly_append(res, poly_create(coeff, deg));
      pp2 = pp2->next;
    }
    ret = poly_addeq(ret, res);
    pp1 = pp1->next;
  }
  return ret;
}

poly poly_create_from_input() {
  COEFF_T coeff;
  DEG_T deg;
  poly p = NULL;
  scanf("%d", &coeff);
  while(coeff != 0) {
    scanf("%d", &deg);
    // printf("coeff, deg: %d, %d", coeff, deg);
    if(p != NULL && p->deg >= deg) {
      printf("Please input elements in increasing order");
      continue;
    }
    p = poly_append(p, poly_create(coeff, deg));
    assert(p != NULL);
    scanf("%d", &coeff);
  }
  return p;
}

void poly_println(poly p) {
  int head = 1;
  while(p != NULL) {
    if(head) {
      printf("%d", p->coeff);
    } else {
      if(p->coeff > 0) 
        printf(" + %d", p->coeff);
      else
        printf(" - %d", -p->coeff);
    }
    if(p->deg != 0) {
      printf("x^%d", p->deg);
    }
    head = 0;
    p = p->next;
  }
  printf("\n");
}

int main() {
  poly p1 = NULL, p2 = NULL, pm = NULL;
  printf("Input polynomial1 [(coeff)(deg), ..., 0]: ");
  p1 = poly_create_from_input();
  printf("polynomial 1: "); poly_println(p1);
  printf("Input polynomial2 [(coeff)(deg), ..., 0]: ");
  p2 = poly_create_from_input();
  pm = pmult(p1, p2);
  printf("polynomial 1: "); poly_println(p1);
  printf("polynomial 2: "); poly_println(p2);
  printf("multiplication result: "); poly_println(pm);
}