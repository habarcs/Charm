#include "itarray.h"
#include "set.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void itarray_remove_subsumed_pairs(ITArray *C) {
  for (int i = 0; i < C->size; i++) {
    if (itarray_is_itpair_subsumed(C, &C->itpairs[i])) {
      itarray_remove(C, i);
      i--;
    }
  }
}

void itarray_remove_low_suport_pairs(ITArray *C, int min_support) {
  for (int i = 0; i < C->size; i++) {
    if (C->itpairs[i].tidset.size < min_support) {
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
void print_closed_itemsets(ITArray *C, bool character, const char *out_file) {
  if (out_file == NULL) {
    return;
  }
  FILE *out = stdout;
  bool should_close = false;
  if (strcmp(out_file, "stdout") != 0) {
    out = fopen(out_file, "w");
    if (out == NULL) {
      fprintf(stderr, "Opening out file failed, writing to stdout instead\n");
      out = stdout;
    }
    should_close = true;
  }

  fprintf(out, "%d Closed itemsets found\n", C->size);
  for (int i = 0; i < C->size; i++) {
    fprintf(out, "itemset: ");
    for (int j = 0; j < C->itpairs[i].itemset.size; j++) {
      if (character) {
        fprintf(out, "%c ", index_to_char(C->itpairs[i].itemset.set[j]));
      } else {
        fprintf(out, "%d ", C->itpairs[i].itemset.set[j]);
      }
    }
    fprintf(out, "\t| tids: ");
    for (int j = 0; j < C->itpairs[i].tidset.size; j++) {
      fprintf(out, "%d ", C->itpairs[i].tidset.set[j]);
    }
    fprintf(out, "\n");
  }
  if (should_close) {
    fclose(out);
  }
}

void add_back_all_frequent_itemsets(ITArray *C) {
  ITArray temp;
  itarray_init(&temp, 1);

  SetHash existing;
  sethash_init(&existing);
  for (int i = 0; i < C->size; i++) {
    sethash_add(&existing, &C->itpairs[i].itemset);
  }

  for (int i = 0; i < C->size; i++) {
    Set *itemset = &C->itpairs[i].itemset;
    Set *tidset = &C->itpairs[i].tidset;
    int num_subsets = 1 << itemset->size;

    for (int j = 1; j < num_subsets - 1; j++) {
      Set subset;
      set_init(&subset, 1);
      for (int k = 0; k < itemset->size; k++) {
        if ((j & (1 << k)) != 0) {
          set_add(&subset, itemset->set[k]);
        }
      }

      if (!sethash_contains(&existing, &subset)) {
        itarray_add(&temp, &subset, tidset);
        Set *copy = malloc(sizeof(Set));
        set_init(copy, subset.size);
        for (int m = 0; m < subset.size; m++) {
          set_add(copy, subset.set[m]);
        }
        sethash_add(&existing, copy);
      } else {
        itarray_add(&temp, &subset, tidset);
      }

      set_free(&subset);
    }
  }

  merge_closed_itemsets_into(&temp, C, false);
  itarray_free(&temp);
  sethash_free(&existing);
}

void merge_closed_itemsets_into(ITArray *from, ITArray *to, bool add_back) {
  if (add_back) {
    add_back_all_frequent_itemsets(from);
    add_back_all_frequent_itemsets(to);
  }
  for (int i = 0; i < from->size; i++) {
    Set *from_itemset = &from->itpairs[i].itemset;
    Set *from_tidset = &from->itpairs[i].tidset;
    bool already_in = false;
    for (int j = 0; j < to->size; j++) {
      Set *to_itemset = &to->itpairs[j].itemset;
      Set *to_tidset = &to->itpairs[j].tidset;
      if (sets_equal(from_itemset, to_itemset)) {
        set_merge_union_sorted(from_tidset, to_tidset);
        already_in = true;
        break;
      }
    }
    if (!already_in) {
      itarray_add(to, from_itemset, from_tidset);
    }
  }
}

void serialize_itarray(const ITArray *data, int min_support, int **buffer,
                       int *bufsize) {
  int total_size = 1 + 1 + (data->size * 2);
  for (int i = 0; i < data->size; i++) {
    total_size += data->itpairs[i].itemset.size;
    total_size += data->itpairs[i].tidset.size;
  }

  *buffer = (int *)malloc(total_size * sizeof(int));
  *bufsize = total_size;

  int index = 0;
  (*buffer)[index++] = min_support;
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

void deserialize_itarray(int *buffer, ITArray *data, int *min_support) {
  int index = 0;
  *min_support = buffer[index++];
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
