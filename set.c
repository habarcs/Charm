#include "set.h"
#include <stdio.h>
#include <stdlib.h>

void set_init(Set *set, int initial_cap) {
  if (initial_cap <= 0) {
    initial_cap = 1;
  }
  set->set = calloc(initial_cap, sizeof(int));
  if (!set->set) {
    fprintf(stderr, "Failed to allocate memory for set\n");
    abort();
  }
  set->cap = initial_cap;
  set->size = 0;
}

void set_free(Set *set) {
  free(set->set);
  set->set = NULL;
  set->cap = 0;
  set->size = 0;
}

void set_add(Set *set, int elem) {
  int pos = set->size;
  for (int i = 0; i < set->size; i++) {
    if (elem == set->set[i]) {
      return; // element was already in set, we don't have to do anything
    }
    if (elem < set->set[i]) {
      pos = i;
      break;
    }
  }
  if (set->size == set->cap) {
    int new_cap = 2 * set->cap;
    int *new = realloc(set->set, new_cap * sizeof(int));
    if (!new) {
      fprintf(stderr, "Failed to allocate memory for set\n");
      abort();
    }
    set->set = new;
    set->cap = new_cap;
  }
  for (int i = set->size - 1; i >= pos; i--) {
    set->set[i + 1] = set->set[i];
  }
  set->set[pos] = elem;
  set->size++;
}

void set_copy(const Set *from, Set *target) {
  set_init(target, from->size);
  for (int i = 0; i < from->size; i++) {
    set_add(target, from->set[i]);
  }
}

void set_union(const Set *a, const Set *b, Set *uni) {
  int i, j;
  i = j = 0;
  set_init(uni, a->size + b->size);
  while (i < a->size && j < b->size) {
    if (a->set[i] < b->set[j]) {
      uni->set[uni->size] = a->set[i];
      uni->size++;
      i++;
    } else if (a->set[i] > b->set[j]) {
      uni->set[uni->size] = b->set[j];
      uni->size++;
      j++;
    } else {
      uni->set[uni->size] = a->set[i];
      uni->size++;
      i++;
      j++;
    }
  }
  for (; i < a->size; i++) {
    uni->set[uni->size++] = a->set[i];
  }
  for (; j < b->size; j++) {
    uni->set[uni->size++] = b->set[j];
  }
}

void set_intersect(const Set *a, const Set *b, Set *inter) {
  set_init(inter, a->size < b->size ? a->size : b->size);
  int i, j;
  i = j = 0;
  while (i < a->size && j < b->size) {
    if (a->set[i] < b->set[j]) {
      i++;
    } else if (a->set[i] > b->set[j]) {
      j++;
    } else {
      inter->set[inter->size] = a->set[i];
      inter->size++;
      i++;
      j++;
    }
  }
}

bool is_subset(const Set *sub, const Set *sup) {
  int i, j;
  i = j = 0;
  while (i < sub->size && j < sup->size) {
    if (sub->set[i] < sup->set[j]) {
      return false;
    } else if (sub->set[i] > sup->set[j]) {
      j++;
    } else {
      i++;
      j++;
    }
  }
  if (i < sub->size) {
    return false;
  }
  return true;
}

bool sets_equal(const Set *a, const Set *b) {
  if (a->size != b->size) {
    return false;
  }
  for (int i = 0; i < a->size; i++) {
    if (a->set[i] != b->set[i]) {
      return false;
    }
  }
  return true;
}

int compare_sets(const Set *a, const Set *b) {
  int i = 0;
  while (i < a->size && i < b->size) {
    if (a->set[i] < b->set[i]) {
      return -1;
    } else if (a->set[i] > b->set[i]) {
      return 1;
    }
    i++;
  }
  if (i < a->size) {
    return -1;
  }
  if (i < b->size) {
    return 1;
  }
  return 0;
}
