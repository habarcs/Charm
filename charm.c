#include "charm.h"
#include "utils.h"
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void charm_extend(ITArray *P, ITArray *C, int min_support, int depth) {
  for (int i = 0; i < P->size; i++) {
    ITArray Pi = {0};
    Set Xi = P->itpairs[i].itemset;
    Set tXi = P->itpairs[i].tidset;

    for (int j = i + 1; j < P->size; j++) {
      const Set Xj = P->itpairs[j].itemset;
      const Set tXj = P->itpairs[j].tidset;
      const Set Xij = set_union(Xi, Xj);
      const Set tXij = set_intersect(tXi, tXj);

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
      charm_extend(&Pi, C, min_support, depth + 1);
    }
    add_itemset_if_not_subsumed(C, (ITPair){Xi, tXi});
  }
}

void enumerate_frequent(const ITPair *P, const ITArray *P_children,
                        int min_support, ITArray *C, int depth) {
#pragma omp parallel for schedule(guided)
  for (int i = 0; i < P_children->size; i++) {
    ITPair *Pi = calloc(1, sizeof(ITPair));
    if (!Pi) {
      fprintf(stderr, "Failed to allocate memory!");
      abort();
    }
    Pi->itemset = set_union(P->itemset, P_children->itpairs[i].itemset);
    if (depth != 0) {
      Pi->tidset = set_intersect(P->tidset, P_children->itpairs[i].tidset);
    } else {
      // this should be only possible if it is the root node because for that
      // the tidset should be the whole transaction set but we only pass an
      // empty set
      Pi->tidset = P_children->itpairs[i].tidset;
    }
    ITArray *Pi_children = calloc(1, sizeof(ITArray));
    if (!Pi_children) {
      fprintf(stderr, "Failed to allcoate memory!");
      abort();
    }

    for (int j = i + 1; j < P_children->size; j++) {
      Set I = set_union(Pi->itemset, P_children->itpairs[j].itemset);
      Set T = set_intersect(Pi->tidset, P_children->itpairs[j].tidset);
      if (T.size >= min_support) {
        out_of_bounds(Pi_children->size);
        Pi_children->itpairs[Pi_children->size++] = (ITPair){I, T};
      }
    }
    // dfs
    enumerate_frequent(Pi, Pi_children, min_support, C, depth + 1);
    free(Pi_children);
#pragma omp critical
    add_itemset_if_not_subsumed(C, *Pi);
    free(Pi);
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

  if (getenv("CHARM_SEQ")) {
    printf("Running Sequentially\n");
    charm_extend(&P, &C, min_support, 0);
  } else {
    printf("Running Parallel\n");
    ITPair root = {0};
    enumerate_frequent(&root, &P, min_support, &C, 0);
    remove_subsumed_sets(&C);
  }
  return C;
}

int main() {
  printf("Max Threads: %d\n", omp_get_max_threads());
  printf("Maximum array size is %d\n", ARRAY_SIZE);
  printf("Maximum length for a dataset line is %d\n", MAX_LINE_LENGTH);
  printf("Maximum number of transactions is %d\n", MAX_TRANSACTIONS);

  const char *filename = "data/small_transactions.dat";
  const bool file_contains_characters = true;
  int num_transactions = 0;
  Set *transactions = read_sets_from_file(filename, &num_transactions,
                                          file_contains_characters);
  int min_support = 3;
  printf("Minimum support is 1%% of the number of transactions %d\n",
         min_support);

  struct timespec start, stop;
  clock_gettime(CLOCK_MONOTONIC, &start);

  // ~~~~~~ Run charm ~~~~~~~
  ITArray C = charm(transactions, num_transactions, min_support);
  // ~~~~~~~~~~~~~~~~~~~~~~~~

  clock_gettime(CLOCK_MONOTONIC, &stop);
  double duration = 1000.0 * stop.tv_sec + 1e-6 * stop.tv_nsec -
                    (1000.0 * start.tv_sec + 1e-6 * start.tv_nsec);
  printf("Charm took %.2f ms to run\n", duration);

  free(transactions);
  print_closed_itemsets(C, file_contains_characters);
}
