#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_SET_SIZE 50
#define MAX_CLOSED_SIZE 100

typedef struct {
  int set[MAX_SET_SIZE];
  int size;
} Set;

typedef struct {
  Set itemset;
  Set tidset;
} ITPair;

typedef struct {
  ITPair itpairs[MAX_CLOSED_SIZE];
  int size;
} ClosedItemsets;

typedef struct PrefixClass {
  ITPair itpair;
  struct PrefixClass *next;
} PrefixClass;

int compare(const void *a, const void *b) { return *(int *)a - *(int *)b; }

Set set_union(Set a, Set b) {
  // this can be done better, because they are ordered
  Set uni = a;
  for (int i = 0; i < b.size; i++) {
    bool exists = false;
    for (int j = 0; j < uni.size; j++) {
      if (b.set[i] == uni.set[j]) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      if (uni.size == MAX_SET_SIZE) {
        fprintf(stderr, "Set is already full, cannot create union");
        exit(1);
      }
      uni.set[uni.size++] = b.set[i];
    }
  }
  qsort(&uni.set, uni.size, sizeof(int), compare);
  return uni;
}

Set set_intersect(Set a, Set b) {
  // maybe can be done better
  Set inter = {0};
  for (int i = 0; i < a.size; i++) {
    for (int j = 0; j < b.size; j++) {
      if (a.set[i] == b.set[j]) {
        inter.set[inter.size++] = a.set[i];
        break;
      }
    }
  }
  qsort(&inter.set, inter.size, sizeof(int), compare);
  return inter;
}

bool is_subset(Set sub, Set sup) {
  // could be faster, because arrays are sorted
  for (int i = 0; i < sub.size; i++) {
    bool exists = false;
    for (int j = 0; j < sup.size; j++) {
      if (sub.set[i] == sup.set[j]) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      return false;
    }
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

void add_itemset_if_not_subsumed(ClosedItemsets *C, ITPair itpair) {
  for (int i = 0; i < C->size; i++) {
    if (C->itpairs[i].tidset.size == itpair.tidset.size &&
        is_subset(itpair.itemset, C->itpairs[i].itemset)) {
      return;
    }
  }
  C->itpairs[C->size++] = itpair;
}

void print_closed_itemsets(ClosedItemsets C, bool character) {
  printf("Closed itemsets found:\n");
  for (int i = 0; i < C.size; i++) {
    for (int j = 0; j < C.itpairs[i].itemset.size; j++) {
      if (character) {
        printf("%c ", C.itpairs[i].itemset.set[j]);
      } else {
        printf("%d ", C.itpairs[i].itemset.set[j]);
      }
    }
    printf("\n");
  }
}

int compare_sets(Set a, Set b) {
  int size = (a.size < b.size) ? a.size : b.size;
  for (int i = 0; i < size; i++) {
    if (a.set[i] == b.set[i]) {
      continue;
    }
    return a.set[i] - b.set[i];
  }
  if (b.size == a.size) {
    return 0;
  } else if (a.size < b.size) {
    return -1;
  }
  return 1;
}

PrefixClass *allocate_itpair(ITPair itpair, PrefixClass *next) {
  PrefixClass *new = malloc(sizeof(PrefixClass));
  if (new == NULL) {
    fprintf(stderr, "Failed to allocate memory for PrefixClass");
    exit(1);
  }
  new->itpair = itpair;
  new->next = next;
  return new;
}

void add_itpair(PrefixClass **head, ITPair itpair) {
  if (*head == NULL) {
    *head = allocate_itpair(itpair, NULL);
    return;
  }

  for (PrefixClass *prev = NULL, *curr = *head; curr != NULL;
       prev = curr, curr = curr->next) {
    int cpr = compare_sets(curr->itpair.itemset, itpair.itemset);
    if (cpr < 0) {
      if (curr->next == NULL) {
        curr->next = allocate_itpair(itpair, NULL);
        return;
      }
      continue;
    } else if (cpr == 0) {
      curr->itpair.tidset = set_union(curr->itpair.tidset, itpair.tidset);
      return;
    } else {
      PrefixClass *tmp = allocate_itpair(itpair, curr);

      if (prev == NULL) {
        *head = tmp;
      } else {
        prev->next = tmp;
      }
      return;
    }
  }
}

void replace_itemset(PrefixClass **head, Set replace, Set with) {
  for (PrefixClass *curr = *head; curr != NULL; curr = curr->next) {
    if (is_subset(replace, curr->itpair.itemset)) {
      curr->itpair.itemset = set_union(curr->itpair.itemset, with);
    }
  }
}

void remove_itpair(PrefixClass **head, Set itemset) {
  if (*head == NULL) {
    return;
  }
  for (PrefixClass *prev = NULL, *curr = *head; curr != NULL;
       prev = curr, curr = curr->next) {
    if (sets_equal(curr->itpair.itemset, itemset)) {
      if (prev != NULL) {
        prev->next = curr->next;
        free(curr);
      } else {
        *head = curr->next;
        free(curr);
      }
      return;
    }
  }
}

void remove_low_support_itemsets(PrefixClass **head, int min_support) {
  if (*head == NULL) {
    return;
  }
  for (PrefixClass *prev = NULL, *curr = *head; curr != NULL;
       prev = curr, curr = curr->next) {
    if (curr->itpair.tidset.size < min_support) {
      if (prev != NULL) {
        prev->next = curr->next;
        free(curr);
        curr = prev;
      } else {
        *head = curr->next;
        free(curr);
        curr = *head;
      }
    }
  }
}

void delete_PrefixClass(PrefixClass **head) {
  if (*head == NULL) {
    return;
  }
  for (PrefixClass *curr = *head; curr != NULL;) {
    PrefixClass *tmp = curr->next;
    free(curr);
    curr = tmp;
  }
  *head = NULL;
}

bool charm_property(PrefixClass **P, PrefixClass **Pi, ITPair I, ITPair J,
                    ITPair XY, int min_support) {
  if (XY.tidset.size >= min_support) {
    if (sets_equal(I.tidset, J.tidset)) {
      remove_itpair(P, J.itemset);
      replace_itemset(Pi, I.itemset, XY.itemset);
      replace_itemset(P, I.itemset, XY.itemset);
      return true;
    }
    if (is_subset(I.tidset, J.tidset)) {
      replace_itemset(Pi, I.itemset, XY.itemset);
      replace_itemset(P, I.itemset, XY.itemset);
      return false;
    }
    if (is_subset(J.tidset, I.tidset)) {
      remove_itpair(P, J.itemset);
      add_itpair(Pi, XY);
      return true;
    }
    if (!sets_equal(I.tidset, J.tidset)) {
      add_itpair(Pi, XY);
      return false;
    }
  }
  return false;
}

void charm_extend(PrefixClass **P, ClosedItemsets *C, int min_support) {
  for (PrefixClass *i = *P; i != NULL; i = i->next) {
    PrefixClass *Pi = NULL;
    Set X = i->itpair.itemset;
    ITPair XY = {0};
    for (PrefixClass *j = *P, *prev = NULL; j != NULL; prev = j, j = j->next) {
      if (compare_sets(i->itpair.itemset, j->itpair.itemset) < 0) {
        X = set_union(X, j->itpair.itemset);
        Set Y = set_intersect(i->itpair.tidset, j->itpair.tidset);
        XY = (ITPair){X, Y};
        bool j_deleted =
            charm_property(P, &Pi, i->itpair, j->itpair, XY, min_support);
        if (j_deleted) {
          if (prev == NULL) {
            fprintf(stderr, "I didn't think this was possible");
            exit(1);
          }
          j = prev;
        }
      }
    }
    if (Pi != NULL) {
      charm_extend(&Pi, C, min_support);
    }
    delete_PrefixClass(&Pi);
    add_itemset_if_not_subsumed(C, XY);
  }
}

ClosedItemsets charm(Set *transactions, int num_transactions, int min_support) {
  ClosedItemsets C = {0};
  PrefixClass *P = NULL;

  for (int i = 0; i < num_transactions; i++) {
    for (int j = 0; j < transactions[i].size; j++) {
      Set itemset = {{transactions[i].set[j]}, 1};
      Set tidset = {{i + 1}, 1};
      ITPair pair = {itemset, tidset};
      add_itpair(&P, pair);
    }
  }
  remove_low_support_itemsets(&P, min_support);

  charm_extend(&P, &C, min_support);

  return C;
}

int main() {
  Set transactions[6] = {
      {{'a', 'c', 't', 'w'}, 4},      {{'c', 'd', 'w'}, 3},
      {{'a', 'c', 't', 'w'}, 4},      {{'a', 'c', 'd', 'w'}, 4},
      {{'a', 'c', 'd', 't', 'w'}, 5}, {{'c', 'd', 't'}, 3}};
  ClosedItemsets C = charm(transactions, 6, 3);
  print_closed_itemsets(C, true);
}
