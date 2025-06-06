#include "itarray.h"
#include "set.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void charm_extend(ITArray *P, ITArray *C, int min_support) {
  for (int i = 0; i < P->size; i++) {
    ITArray Pi;
    itarray_init(&Pi, 1);

    Set Xi;
    set_copy(&P->itpairs[i].itemset, &Xi);
    Set tXi;
    set_copy(&P->itpairs[i].tidset, &tXi);

    for (int j = i + 1; j < P->size; j++) {
      Set Xj;
      set_copy(&P->itpairs[j].itemset, &Xj);
      Set tXj;
      set_copy(&P->itpairs[j].tidset, &tXj);

      Set Xij;
      set_union(&Xi, &Xj, &Xij);
      Set tXij;
      set_intersect(&tXi, &tXj, &tXij);

      if (tXij.size >= min_support) {
        if (sets_equal(&tXi, &tXj)) {
          itarray_replace_with(P, &Xi, &Xij);
          itarray_replace_with(&Pi, &Xi, &Xij);
          set_free(&Xi);
          set_copy(&Xij, &Xi);
          set_free(&tXi);
          set_copy(&tXij, &tXi);
          itarray_remove(P, j);
          j--;
        } else {
          if (is_subset(&tXi, &tXj)) {
            itarray_replace_with(P, &Xi, &Xij);
            itarray_replace_with(&Pi, &Xi, &Xij);
            set_free(&Xi);
            set_copy(&Xij, &Xi);
            set_free(&tXi);
            set_copy(&tXij, &tXi);
          } else {
            itarray_add(&Pi, &Xij, &tXij);
            qsort(Pi.itpairs, Pi.size, sizeof(ITPair), compare_itpairs);
            // qsort(Pi.itpairs, Pi.size, sizeof(ITPair),
            // compare_itpairs_support);
          }
        }
      }
      set_free(&Xj);
      set_free(&tXj);
      set_free(&Xij);
      set_free(&tXij);
    }
    if (Pi.size > 0) {
      charm_extend(&Pi, C, min_support);
    }
    itarray_free(&Pi);
    if (!itarray_is_itpair_subsumed(C, &(ITPair){Xi, tXi})) {
      itarray_add(C, &Xi, &tXi);
    }
    set_free(&Xi);
    set_free(&tXi);
  }
}

void enumerate_frequent(const ITPair *P, const ITArray *P_children,
                        int min_support, ITArray *C, int depth) {
  #pragma omp parallel for schedule(guided)
  for (int i = 0; i < P_children->size; i++) {
    ITPair Pi = {{0}, {0}};
    set_union(&P->itemset, &P_children->itpairs[i].itemset, &Pi.itemset);
    if (depth != 0) {
      set_intersect(&P->tidset, &P_children->itpairs[i].tidset, &Pi.tidset);
    } else {
      // this should only be possible if it is the root node. Normally for the
      // root node the tidset is the whole transaction set but we only pass an
      // empty set to save space
      set_copy(&P_children->itpairs[i].tidset, &Pi.tidset);
    }
    ITArray Pi_children;
    itarray_init(&Pi_children, P_children->size - i);

    for (int j = i + 1; j < P_children->size; j++) {
      Set I;
      set_union(&Pi.itemset, &P_children->itpairs[j].itemset, &I);
      Set T;
      set_intersect(&Pi.tidset, &P_children->itpairs[j].tidset, &T);
      if (T.size >= min_support) {
        itarray_add(&Pi_children, &I, &T);
      }
      set_free(&I);
      set_free(&T);
    }
    // dfs
    enumerate_frequent(&Pi, &Pi_children, min_support, C, depth + 1);

    #pragma omp critical
    if (!itarray_is_itpair_subsumed(C, &Pi)) {
      itarray_add(C, &Pi.itemset, &Pi.tidset);
    }

    itarray_free(&Pi_children);
    set_free(&Pi.itemset);
    set_free(&Pi.tidset);
  }
}

ITArray charm(Set *transactions, int num_transactions, int min_support,
              int tid_start) {
  ITArray C;
  itarray_init(&C, 1 + num_transactions / 1000);
  ITArray P;
  itarray_init(&P, 1 + num_transactions / 1000);

  for (int i = 0; i < num_transactions; i++) {
    for (int j = 0; j < transactions[i].size; j++) {
      int item = transactions[i].set[j];
      int tid = tid_start + i;
      bool already_added = false;
      for (int k = 0; k < P.size; k++) {
        ITPair *itpair = &P.itpairs[k];
        if (itpair->itemset.size == 1 && itpair->itemset.set[0] == item) {
          set_add(&itpair->tidset, tid);
          already_added = true;
          break;
        }
      }
      if (!already_added) {
        int pos = itarray_add(&P, NULL, NULL);
        set_add(&P.itpairs[pos].itemset, item);
        set_add(&P.itpairs[pos].tidset, tid);
      }
    }
  }
  itarray_remove_low_suport_pairs(&P, min_support);
  qsort(P.itpairs, P.size, sizeof(ITPair), compare_itpairs);
  // qsort(P.itpairs, P.size, sizeof(ITPair), compare_itpairs_support);

  bool openmp = false;
  char *openmp_env = getenv("CHARM_OPENMP");
  if (openmp_env != NULL) {
    openmp = atoi(openmp_env) != 0;
  }

  if (openmp) {
    printf("Running Parallel\n");
    ITPair root = {{0}, {0}};
    enumerate_frequent(&root, &P, min_support, &C, 0);
    itarray_remove_subsumed_pairs(&C);
  } else {
    printf("Running Sequentially\n");
    charm_extend(&P, &C, min_support);
  }
  itarray_free(&P);
  return C;
}
