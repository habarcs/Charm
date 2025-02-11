#include "charm_functions.h"
#include "itarray.h"
#include "utils.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

  int rank = 0, size = 0;
  MPI_Init(NULL, NULL);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  printf("My rank is %d/%d\n", rank + 1, size);
  int min_support = 3;
  int local_min_support = min_support / size;
  // get and divide the dataset
  int num_transactions = 0, partition_size = 0, local_size = 0;
  Set *transactions = read_sets_from_file_start_end(
      "data/small_transactions.dat", &num_transactions, rank, size,
      &partition_size, &local_size, true);
  printf("Process %d has the %d transactions\n", rank, local_size);
  for (int i = 0; i < local_size; i++) {
    Set *t = &transactions[i];
    printf("\nItemset (size %d): ", t->size);
    for (int j = 0; j < t->size; j++) {
      printf(" %c ", index_to_char(t->set[j]));
    }
  }
  printf("\n");

  // compute first iteration in serial ??? No, i think that should already be
  // parallelized
  int tid_start = rank * partition_size + 1;
  ITArray C = charm(transactions, local_size, local_min_support, tid_start);

  print_closed_itemsets(&C, true);

  free(transactions);
  MPI_Finalize();

  return 0;
}
