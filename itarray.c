#include "itarray.h"
#include "set.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void itarray_init(ITArray *array, int initial_cap) {
  if (initial_cap <= 0) {
    initial_cap = 1;
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
  array->itpairs = NULL;
  array->cap = 0;
  array->size = 0;
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

int itarray_add(ITArray *array, const Set *itemset, const Set *tidset) {
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
  int pos = array->size;
  array->size++;
  if (itemset) {
    set_copy(itemset, &array->itpairs[pos].itemset);
  } else {
    set_init(&array->itpairs[pos].itemset, 1);
  }
  if (tidset) {
    set_copy(tidset, &array->itpairs[pos].tidset);
  } else {
    set_init(&array->itpairs[pos].tidset, 1);
  }
  return pos;
}

bool itarray_is_itpair_subsumed(ITArray *C, const ITPair *itpair) {
  for (int i = 0; i < C->size; i++) {
    if (C->itpairs[i].tidset.size == itpair->tidset.size &&
        is_subset(&itpair->itemset, &C->itpairs[i].itemset)) {
      return true;
    }
  }
  return false;
}

void itarray_remove(ITArray *P, int pos) {
  if (pos >= P->size) {
    return;
  }
  set_free(&P->itpairs[pos].itemset);
  set_free(&P->itpairs[pos].tidset);
  for (int i = pos; i < P->size - 1; i++) {
    P->itpairs[i] = P->itpairs[i + 1];
  }
  P->size--;
  // TODO maybe shrink array if size is much lower then cap
}

void itarray_remove_subsumed_sets(ITArray *C) {
  for (int i = 0; i < C->size; i++) {
    if (itarray_is_itpair_subsumed(C, &C->itpairs[i])) {
      itarray_remove(C, i);
      i--;
    }
  }
}

void itarray_replace_with(ITArray *P, const Set *it, const Set *with) {
  for (int i = 0; i < P->size; i++) {
    if (is_subset(it, &P->itpairs[i].itemset)) {
      set_add_all(with, &P->itpairs[i].itemset);
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
    printf("\t| tids: ");
    for (int j = 0; j < C->itpairs[i].tidset.size; j++) {
      printf("%d ", C->itpairs[i].tidset.set[j]);
    }
    printf("\n");
  }
}

void merge_closed_itemsets(const ITArray **Cs, int size, ITArray *C,
                           int min_support) {
  itarray_init(C, size * 10);

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < Cs[i]->size; j++) {
      bool already_in = false;
      for (int k = 0; k < C->size; k++) {
        if (sets_equal(&Cs[i]->itpairs[j].itemset, &C->itpairs[k].itemset)) {
          already_in = true;
          set_add_all(&Cs[i]->itpairs[j].tidset, &C->itpairs[k].tidset);
        }
      }
      if (!already_in) {
        itarray_add(C, &Cs[i]->itpairs[j].itemset, &Cs[i]->itpairs[j].tidset);
      }
    }
  }

  itarray_remove_subsumed_sets(C);

  // remove low support elements
  for (int i = 0; i < C->size; i++) {
    if (C->itpairs[i].tidset.size < min_support) {
      itarray_remove(C, i);
      i--;
    }
  }
}

void serialize_itarray(const ITArray *data, int **buffer, int *bufsize) {
  int total_size = 1 + (data->size * 2);
  for (int i = 0; i < data->size; i++) {
    total_size += data->itpairs[i].itemset.size;
    total_size += data->itpairs[i].tidset.size;
  }

  *buffer = (int *)malloc(total_size * sizeof(int));
  *bufsize = total_size;

  int index = 0;
  (*buffer)[index++] = data->size;

  for (int i = 0; i < data->size; i++) {
    ITPair *pair = &data->itpairs[i];

    (*buffer)[index++] = pair->itemset.size;
    for (int j = 0; j < pair->itemset.size; j++) {
      (*buffer)[index++] = pair->itemset.set[j];
    }

    (*buffer)[index++] = pair->tidset.size;
    for (int j = 0; j < pair->tidset.size; j++) {
      (*buffer)[index++] = pair->tidset.set[j];
    }
  }
}

void deserialize_itarray(int *buffer, ITArray *data) {
  int index = 0;
  int num_itpairs = buffer[index++];

  itarray_init(data, num_itpairs);

  for (int i = 0; i < num_itpairs; i++) {
    int itemset_size = buffer[index++];
    Set itemset;
    set_init(&itemset, itemset_size);
    for (int j = 0; j < itemset_size; j++) {
      set_add(&itemset, buffer[index++]);
    }

    int tidset_size = buffer[index++];
    Set tidset;
    set_init(&tidset, tidset_size);
    for (int j = 0; j < tidset_size; j++) {
      set_add(&tidset, buffer[index++]);
    }

    itarray_add(data, &itemset, &tidset);
    set_free(&itemset);
    set_free(&tidset);
  }
}
