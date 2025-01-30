#include "itarray.h"
#include "set.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void itarray_init(ITArray *array, int initial_cap) {
  if (initial_cap <= 0) {
    fprintf(stderr, "itarray_init, initial_cap has to be bigger then zero");
    abort();
  }
  array->itpairs = calloc(initial_cap, sizeof(ITPair));
  if (!array->itpairs) {
    fprintf(stderr, "Failed to allocate memory for itarray");
    abort();
  }
  array->cap = initial_cap;
  array->size = 0;
}

void itarray_free(ITArray *array) {
  for (int i = 0; i < array->size; i++) {
    set_free(&array->itpairs[i].itemset);
    set_free(&array->itpairs[i].tidset);
  }
  free(array->itpairs);
}

int compare_itpairs(const void *a, const void *b) {
  return compare_sets(&((ITPair *)a)->itemset, &((ITPair *)b)->itemset);
}

int compare_itpairs_support(const void *a, const void *b) {
  if (((ITPair *)a)->tidset.size > ((ITPair *)b)->tidset.size) {
    return 1;
  }
  if (((ITPair *)a)->tidset.size < ((ITPair *)b)->tidset.size) {
    return -1;
  }
  return 0;
}

void add_itpair(ITArray *array, ITPair itpair) {
  if (array->size == array->cap) {
    int new_cap = 2 * array->cap;
    ITPair *new = realloc(array->itpairs, new_cap * sizeof(ITPair));
    if (!new) {
      fprintf(stderr, "Failed to allocate memory for ITArray");
      abort();
    }
    array->itpairs = new;
    array->cap = new_cap;
  }
  array->itpairs[array->size] = itpair;
  array->size++;
}

void add_itemset_if_not_subsumed(ITArray *C, ITPair itpair) {
  for (int i = 0; i < C->size; i++) {
    if (C->itpairs[i].tidset.size == itpair.tidset.size &&
        is_subset(&itpair.itemset, &C->itpairs[i].itemset)) {
      return;
    }
  }
  add_itpair(C, itpair);
}

void remove_itpair(ITArray *P, int pos) {
  if (pos >= P->size) {
    return;
  }
  for (int i = pos; i < P->size - 1; i++) {
    P->itpairs[i] = P->itpairs[i + 1];
  }
  P->size--;
  // TODO maybe shrink array if size is much lower then cap
}

void remove_subsumed_sets(ITArray *C) {
  for (int i = 0; i < C->size; i++) {
    bool subsumed = false;
    for (int j = i + 1; j < C->size; j++) {
      if (C->itpairs[i].tidset.size == C->itpairs[j].tidset.size &&
          is_subset(&C->itpairs[i].itemset, &C->itpairs[j].itemset)) {
        subsumed = true;
        break;
      }
    }
    if (subsumed) {
      remove_itpair(C, i);
      i--;
    }
  }
}

void replace_with(ITArray *P, Set it, Set with) {
  for (int i = 0; i < P->size; i++) {
    if (is_subset(&it, &P->itpairs[i].itemset)) {
      for (int j = 0; j < with.size; j++) {
        set_add(&P->itpairs[0].itemset, with.set[j]);
      }
    }
  }
}

// we can convert back to char the items
void print_closed_itemsets(ITArray *C, bool character) {
  printf("Closed itemsets found:\n");
  for (int i = 0; i < C->size; i++) {
    printf("itemset: ");
    for (int j = 0; j < C->itpairs[i].itemset.size; j++) {
      if (character) {
        printf("%c ", index_to_char(C->itpairs[i].itemset.set[j]));
      } else {
        printf("%d ", C->itpairs[i].itemset.set[j]);
      }
    }
    printf("\t\t| tids: ");
    for (int j = 0; j < C->itpairs[i].tidset.size; j++) {
      printf("%d ", C->itpairs[i].tidset.set[j]);
    }
    printf("\n");
  }
}
