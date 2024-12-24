#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int compare(const void *a, const void *b) { return *(int *)a - *(int *)b; }

void out_of_bounds(int i) {
  if (i >= ARRAY_SIZE) {
    fprintf(stderr, "Array out of bounds, exiting");
    abort();
  }
}

Set set_union(Set a, Set b) {
  Set uni = {0};
  int i, j;
  i = j = 0;
  while (i < a.size && j < b.size) {
    out_of_bounds(uni.size);
    if (a.set[i] < b.set[j]) {
      uni.set[uni.size] = a.set[i];
      uni.size++;
      i++;
    } else if (a.set[i] > b.set[j]) {
      uni.set[uni.size] = b.set[j];
      uni.size++;
      j++;
    } else {
      uni.set[uni.size] = a.set[i];
      uni.size++;
      i++;
      j++;
    }
  }
  for (; i < a.size; i++) {
    out_of_bounds(uni.size);
    uni.set[uni.size++] = a.set[i];
  }
  for (; j < b.size; j++) {
    out_of_bounds(uni.size);
    uni.set[uni.size++] = b.set[j];
  }
  return uni;
}

Set set_intersect(Set a, Set b) {
  Set inter = {0};
  int i, j;
  i = j = 0;
  while (i < a.size && j < b.size) {
    if (a.set[i] < b.set[j]) {
      i++;
    } else if (a.set[i] > b.set[j]) {
      j++;
    } else {
      out_of_bounds(inter.size);
      inter.set[inter.size] = a.set[i];
      inter.size++;
      i++;
      j++;
    }
  }
  return inter;
}

bool is_subset(Set sub, Set sup) {
  int i, j;
  i = j = 0;
  while (i < sub.size && j < sup.size) {
    if (sub.set[i] < sup.set[j]) {
      return false;
    } else if (sub.set[i] > sup.set[j]) {
      j++;
    } else {
      i++;
      j++;
    }
  }
  if (i < sub.size) {
    return false;
  }
  return true;
}

bool sets_equal(Set a, Set b) {
  if (a.size != b.size) {
    return false;
  }
  for (int i = 0; i < a.size; i++) {
    if (a.set[i] != b.set[i]) {
      return false;
    }
  }
  return true;
}

int compare_sets(Set a, Set b) {
  int i = 0;
  while (i < a.size && i < b.size) {
    if (a.set[i] < b.set[i]) {
      return -1;
    } else if (a.set[i] > b.set[i]) {
      return 1;
    }
    i++;
  }
  if (i < a.size) {
    return -1;
  }
  if (i < b.size) {
    return 1;
  }
  return 0;
}

int compare_itpairs(const void *a, const void *b) {
  return compare_sets(((ITPair *)a)->itemset, ((ITPair *)b)->itemset);
}

void add_itemset_if_not_subsumed(ITArray *C, ITPair itpair) {
  for (int i = 0; i < C->size; i++) {
    if (C->itpairs[i].tidset.size == itpair.tidset.size &&
        is_subset(itpair.itemset, C->itpairs[i].itemset)) {
      return;
    }
  }
  out_of_bounds(C->size);
  C->itpairs[C->size++] = itpair;
}

void remove_itpair(ITArray *P, int pos) {
  if (pos >= P->size) {
    return;
  }
  for (int i = pos; i < P->size - 1; i++) {
    P->itpairs[i] = P->itpairs[i + 1];
  }
  P->size--;
}

void replace_with(ITArray *P, Set it, Set with) {
  for (int i = 0; i < P->size; i++) {
    if (is_subset(it, P->itpairs[i].itemset)) {
      P->itpairs[i].itemset = set_union(P->itpairs[i].itemset, with);
    }
  }
}

void print_closed_itemsets(ITArray C, bool character) {
  printf("Closed itemsets found:\n");
  for (int i = 0; i < C.size; i++) {
    printf("itemset: ");
    for (int j = 0; j < C.itpairs[i].itemset.size; j++) {
      if (character) {
        printf("%c ", C.itpairs[i].itemset.set[j]);
      } else {
        printf("%d ", C.itpairs[i].itemset.set[j]);
      }
    }
    printf("\t| tids: ");
    for (int j = 0; j < C.itpairs[i].tidset.size; j++) {
      printf("%d ", C.itpairs[i].tidset.set[j]);
    }
    printf("\n");
  }
}
