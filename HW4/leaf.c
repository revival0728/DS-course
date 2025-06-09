// Data Structure HW4
// Example Input:
//   (1(2(3(4,5),6),7(8)))
// Example Output:
//   The input btree has 4 leaf nodes

#include <stdio.h>
#include <stdlib.h>

typedef struct btree_t {
  int id;
  struct btree_t *lc, *rc;
} btree_t, *btree;

void* malloc_s(size_t size) {
  void *m = malloc(size);
  if(m == NULL) {
    printf("Allocation Failed.");
    return NULL;
  }
  return m;
}

btree create_btree() {
  btree tree = malloc_s(sizeof(btree_t));
  tree->id = 0;
  tree->lc = NULL;
  tree->rc = NULL;
  return tree;
}

void free_btree(btree cur) {
  if(cur == NULL) return;
  free_btree(cur->lc);
  free_btree(cur->rc);
  free(cur);
}

void invalid_btree_input() {
  printf("Invalid input pattern\n");
  return;
}

// @return if needs to read right node (1)
int _read_btree(btree parent) {
  if(!parent) {
    invalid_btree_input();
    return 0;
  }
  char c;
  int ic, nid = 0;
  while((ic = getchar()) != EOF) {
    c = ic;
    if(c == '(') {
      parent->lc = create_btree();
      if(_read_btree(parent->lc) == 1) {
        parent->rc = create_btree();
        if(_read_btree(parent->rc) == 1) {
          invalid_btree_input();
          return 0;
        }
      }
    } else if('0' <= c && c <= '9') {
      parent->id *= 10;
      parent->id += c - '0';
    } else if(c == ',') {
      return 1;
    } else if(c == ')') {
      break;
    } else if(c == ' ') {
      continue;
    } else {
      invalid_btree_input();
      return 1;
    }
  }
  return ic == EOF;
}

btree read_btree() {
  printf("Please input btree using list representation: ");
  if(getchar() != '(') {
    invalid_btree_input();
    return NULL;
  }
  btree ret = create_btree();
  if(_read_btree(ret) == 1) {
    invalid_btree_input();
    return NULL;
  }
  return ret;
}

int count_leaf(btree cur) {
  if(cur == NULL) return 0;
  if(!cur->lc && !cur->rc) return 1;
  return count_leaf(cur->lc) + count_leaf(cur->rc);
}

int main() {
  btree tree = read_btree();
  if(tree == NULL) return 0;
  printf("The input btree has %d leaf nodes", count_leaf(tree));
  free_btree(tree);
}