#include <stdbool.h>
#include <stdlib.h>
#include <utils.h>
#include <charm.h>

void charm_extend(ITArray *P, ITArray *C, int min_support) {
  for (int i = 0; i < P->size; i++) {
    ITArray Pi = {0};
    Set X = P->itpairs[i].itemset;
    Set tX = P->itpairs[i].tidset;
    for (int j = i + 1; j < P->size; j++) {
      X = set_union(X, P->itpairs[j].itemset);
      tX = set_intersect(tX, P->itpairs[j].tidset);
      Set Y = set_intersect(P->itpairs[i].tidset, P->itpairs[j].tidset);
      if (Y.size >= min_support) {
        if (sets_equal(P->itpairs[i].tidset, P->itpairs[j].tidset)) {
          remove_itpair(P, j);
          j--;
          replace_with(P, P->itpairs[i].itemset, X);
          replace_with(&Pi, P->itpairs[i].itemset, X);
        } else if (is_subset(P->itpairs[i].tidset, P->itpairs[j].tidset)) {
          replace_with(P, P->itpairs[i].itemset, X);
          replace_with(&Pi, P->itpairs[i].itemset, X);
        } else if (is_subset(P->itpairs[j].tidset, P->itpairs[i].tidset)) {
          remove_itpair(P, j);
          j--;
          Pi.itpairs[Pi.size++] = (ITPair){X, Y};
          qsort(&Pi.itpairs, Pi.size, sizeof(ITPair), compare_itpairs);
        } else if (!sets_equal(P->itpairs[i].tidset, P->itpairs[j].tidset)) {
          Pi.itpairs[Pi.size++] = (ITPair){X, Y};
          qsort(&Pi.itpairs, Pi.size, sizeof(ITPair), compare_itpairs);
        }
      }
    }
    if (Pi.size > 0) {
      charm_extend(&Pi, C, min_support);
    }
    add_itemset_if_not_subsumed(C, (ITPair){X, tX});
  }
}

ITArray charm(Set *transactions, int num_transactions, int min_support) {
  ITArray C = {0};
  ITArray P = {0};

  for (int i = 0; i < num_transactions; i++) {
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
