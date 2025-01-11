#include "charm_functions.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("Maximum array size is %d\n", ARRAY_SIZE);
  printf("Maximum length for a dataset line is %d\n", MAX_LINE_LENGTH);
  printf("Maximum number of transactions is %d\n", MAX_TRANSACTIONS);
  const char *filename = "data/retail.dat";
  const bool file_contains_characters = false;
  int num_transactions = 0;
  Set *transactions = read_sets_from_file(filename, &num_transactions, file_contains_characters);
  int min_support = num_transactions / 100;
  printf("Minimum support is 1%% of the number of transactions %d\n", min_support);
  ITArray C = charm(transactions, num_transactions, min_support);
  free(transactions);
  print_closed_itemsets(C, file_contains_characters);
}
