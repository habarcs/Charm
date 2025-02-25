#include "charm_functions.h"
#include "itarray.h"
#include "set.h"
#include "utils.h"
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  printf("Max Threads: %d\n", omp_get_max_threads());
  printf("Maximum length for a dataset line is %d\n", MAX_LINE_LENGTH);
  printf("Maximum number of transactions is %d\n", MAX_TRANSACTIONS);
  const char *filename = "data/retail.dat";
  const bool file_contains_characters = false;
  int num_transactions = 0;
  Set *transactions = read_sets_from_file(filename, &num_transactions,
                                          file_contains_characters);
  int min_support = num_transactions / 100;
  printf("Minimum support is 1%% of the number of transactions %d\n",
         min_support);

  struct timespec start, stop;
  clock_gettime(CLOCK_MONOTONIC, &start);

  // ~~~~~~ Run charm ~~~~~~~
  ITArray C = charm(transactions, num_transactions, min_support, 1);
  // ~~~~~~~~~~~~~~~~~~~~~~~~

  clock_gettime(CLOCK_MONOTONIC, &stop);
  double duration = 1000.0 * stop.tv_sec + 1e-6 * stop.tv_nsec -
                    (1000.0 * start.tv_sec + 1e-6 * start.tv_nsec);
  printf("Charm took %.2f ms to run\n", duration);

  for (int i = 0; i < num_transactions; i++) {
    set_free(&transactions[i]);
  }
  free(transactions);

  qsort(C.itpairs, C.size, sizeof(ITPair), compare_itpairs);
  print_closed_itemsets(&C, file_contains_characters);
  itarray_free(&C);
}
