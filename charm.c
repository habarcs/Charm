#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define ARRAY_SIZE 100

typedef struct {
  int set[ARRAY_SIZE];
  int size;
} Set;

typedef struct {
  Set itemset;
  Set tidset;
} ITPair;

typedef struct {
  ITPair itpairs[ARRAY_SIZE];
  int size;
} ITArray;

int compare(const void *a, const void *b) { return *(int *)a - *(int *)b; }

void out_of_bounds(int i) {
  if (i >= ARRAY_SIZE) {
    fprintf(stderr, "Array out of bounds, exiting");
    abort();
  }
}

void print_set(Set s, bool character) {
  for (int i = 0; i < s.size; i++) {
    if (character) {
      printf("item: %c\n", s.set[i]);
    } else {
      printf("tid: %d\n", s.set[i]);
    }
  }
  printf("\n");
}

void print_sets(Set a, Set b, char* message, bool itemset) {
  printf("%s\n", message);
  print_set(a, itemset);
  print_set(b, itemset);
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
  // maybe can be done better
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
    if (C->itpairs[i].tidset.size == itpair.tidset.size &&
        is_subset(C->itpairs[i].itemset, itpair.itemset)) {
      C->itpairs[i] = itpair;
      return;
    }
  }
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
    printf("{");
    for (int j = 0; j < C.itpairs[i].itemset.size; j++) {
      if (character) {
        printf(" %c ", C.itpairs[i].itemset.set[j]);
      } else {
        printf(" %d ", C.itpairs[i].itemset.set[j]);
      }
    }
    printf("} with support %d\n", C.itpairs[i].tidset.size);
  }
}

void charm_property(ITArray *P, ITArray *Pi, Set X, Set Y) {

}

void charm_extend(ITArray *P, ITArray *C, int min_support) {
  for (int i = 0; i < P->size; i++) {
    ITArray Pi = {0};
    Set Xi = P->itpairs[i].itemset;
    Set tXi = P->itpairs[i].tidset;
    for (int j = i + 1; j < P->size; j++) {
      Set Xj = set_union(Xi, P->itpairs[j].itemset);
      Set tXj = set_intersect(tXi, P->itpairs[j].tidset);
      // charm_property(P, &Pi, X, Y);
      if (tXj.size >= min_support) {
        if (sets_equal(tXi, P->itpairs[j].tidset)) {
          remove_itpair(P, j);
          j--;
          replace_with(P, Xi, Xj);
          replace_with(&Pi, Xi, Xj);
        } else if (is_subset(tXi, P->itpairs[j].tidset)) {
          replace_with(P, Xi, Xj);
          replace_with(&Pi, Xi, Xj);
        } else if (is_subset(P->itpairs[j].tidset, tXi)) {
          remove_itpair(P, j);
          j--;
          Pi.itpairs[Pi.size++] = (ITPair){Xj, tXj};
          qsort(&Pi.itpairs, Pi.size, sizeof(ITPair), compare_itpairs);
        } else if (!sets_equal(tXi, P->itpairs[j].tidset)) {
          Pi.itpairs[Pi.size++] = (ITPair){Xj, tXj};
          qsort(&Pi.itpairs, Pi.size, sizeof(ITPair), compare_itpairs);
        }
      }
    }
    if (Pi.size > 0) {
      printf("Not finished yet, running again\n");
      charm_extend(&Pi, C, min_support);
    }
    add_itemset_if_not_subsumed(C, (ITPair){Xi, tXi});
  }
}

ITArray charm(Set *transactions, int num_transactions, int min_support) {
  ITArray C = {0};
  ITArray P = {0};

  for (int i = 0; i < num_transactions; i++) {
    printf("Iteranting through transaction number: %d\n", i);
    for (int j = 0; j < transactions[i].size; j++) {
      Set item = {{transactions[i].set[j]}, 1};
      Set tid = {{i + 1}, 1};
      bool already_added = false;
      for (int k = 0; k < P.size; k++) {
        if (sets_equal(P.itpairs[k].itemset, item)) {
          P.itpairs[k].tidset = set_union(P.itpairs[k].tidset, tid);
          already_added = true;
        }
      }
      if (!already_added) {
        out_of_bounds(P.size);
        P.itpairs[P.size++] = (ITPair){item, tid};
      }
    }
  }
  for (int i = 0; i < P.size; i++) {
    if (P.itpairs[i].tidset.size < min_support) {
      printf("Removing a tidset that is less than the min support");
      remove_itpair(&P, i);
      i--;
    }
  }
  qsort(&P.itpairs, P.size, sizeof(ITPair), compare_itpairs);

  charm_extend(&P, &C, min_support);
  return C;
}

int main() {
  Set transactions[6] = {
      {{'a', 'c', 't', 'w'}, 4},      {{'c', 'd', 'w'}, 3},
      {{'a', 'c', 't', 'w'}, 4},      {{'a', 'c', 'd', 'w'}, 4},
      {{'a', 'c', 'd', 't', 'w'}, 5}, {{'c', 'd', 't'}, 3}};
  ITArray C = charm(transactions, 6, 3);
  print_closed_itemsets(C, true);
}
