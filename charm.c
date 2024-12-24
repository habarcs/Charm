#include "charm.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void charm_extend(ITArray *P, ITArray *C, int min_support) {
  for (int i = 0; i < P->size; i++) {
    ITArray Pi = {0};
    Set Xi = P->itpairs[i].itemset;
    Set tXi = P->itpairs[i].tidset;

    for (int j = i + 1; j < P->size; j++) {
      const Set Xj = P->itpairs[j].itemset;
      const Set tXj = P->itpairs[j].tidset;
      Set Xij = set_union(Xi, Xj);
      Set tXij = set_intersect(tXi, tXj);

      if (tXij.size >= min_support) {
        if (sets_equal(tXi, tXj)) {
          replace_with(P, Xi, Xij);
          replace_with(&Pi, Xi, Xij);
          Xi = Xij;
          tXi = tXij;
          remove_itpair(P, j);
          j--;
        } else {
          if (is_subset(tXi, tXj)) {
            replace_with(P, Xi, Xij);
            replace_with(&Pi, Xi, Xij);
            Xi = Xij;
            tXi = tXij;
          } else {
            out_of_bounds(Pi.size);
            Pi.itpairs[Pi.size++] = (ITPair){Xij, tXij};
            qsort(&Pi.itpairs, Pi.size, sizeof(ITPair), compare_itpairs);
          }
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
    printf("Iterating through transaction number: %d\n", i + 1);
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
      printf("Removing a itemset that is less than the min support");
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
