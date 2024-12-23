#include <charm.h>

void charm_extend(ITArray *P, ITArray *C) {
  for (int i = 0; i < P->size; i++) {
    ITArray Pi = {0};
    Set Xi = P->itpairs[i].itemset;
    Set tXi = P->itpairs[i].tidset;
    Set X = Xi;
    Set Y = tXi;
    for (int j = i + 1; j < P->size; j++) {
      Set Xj = P->itpairs[j].itemset;
      Set tXj = P->itpairs[j].tidset;
      X = set_union(Xi, Xj);
      Y = set_intersect(tXi, tXj);
      // charm_property(P, &Pi, X, Y);
      if (Y.size >= MIN_SUPPORT) {
        if (sets_equal(tXi, tXj)) {
          remove_itpair(P, j);
          j--;
          replace_with(P, Xi, X);
          replace_with(&Pi, Xi, X);
        } else if (is_subset(tXi, tXj)) {
          replace_with(P, Xi, X);
          replace_with(&Pi, Xi, X);
        } else if (is_subset(tXj, tXi)) {
          remove_itpair(P, j);
          j--;
          Pi.itpairs[Pi.size++] = (ITPair){X, Y};
          qsort(&Pi.itpairs, Pi.size, sizeof(ITPair), compare_itpairs_by_support);
        } else if (!sets_equal(tXi, tXj)) {
          Pi.itpairs[Pi.size++] = (ITPair){X, Y};
          qsort(&Pi.itpairs, Pi.size, sizeof(ITPair), compare_itpairs_by_support);
        }
      }
    }
    if (Pi.size > 0) {
      charm_extend(&Pi, C);
    }
    add_itemset_if_not_subsumed(C, (ITPair){X, Y});
  }
}

ITArray charm(Set *transactions, int num_transactions) {
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
    if (P.itpairs[i].tidset.size < MIN_SUPPORT) {
      remove_itpair(&P, i);
      i--;
    }
  }
  qsort(&P.itpairs, P.size, sizeof(ITPair), compare_itpairs_by_support);

  charm_extend(&P, &C);
  return C;
}

int main() {
  Set transactions[6] = {
      {{'a', 'c', 't', 'w'}, 4},      {{'c', 'd', 'w'}, 3},
      {{'a', 'c', 't', 'w'}, 4},      {{'a', 'c', 'd', 'w'}, 4},
      {{'a', 'c', 'd', 't', 'w'}, 5}, {{'c', 'd', 't'}, 3}};
  ITArray C = charm(transactions, 6);
  print_closed_itemsets(C, true);
}
