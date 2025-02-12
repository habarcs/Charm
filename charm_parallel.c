#include "charm_functions.h"
#include "utils.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

  int rank = 0, size = 0;
  MPI_Init(NULL, NULL);

  double start_time = 0, end_time = 0, total_time = 0;

  start_time = MPI_Wtime();

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  printf("My rank is %d/%d\n", rank, size);

  char *data_path = getenv("DATA_PATH");
  char *data_file = getenv("DATA_FILE");
  char *data_path_file;

  if (data_path == NULL || data_file == NULL) {
    printf("Data path and file must be defined!");
    return -1;
  } else {
    asprintf(&data_path_file, "%s/%s", data_path, data_file);
    printf("Data file path is %s", data_path_file);
  }

  bool characters = getenv("DATA_CHARACTERS");
  if (characters == NULL) {
    characters = true;
  }

  // get and divide the dataset
  int num_transactions = 0, partition_size = 0, local_size = 0;
  Set *transactions = read_sets_from_file_start_end(
      data_path_file, &num_transactions, rank, size,
      &partition_size, &local_size, characters);
  printf("Process %d has the %d transactions\n", rank, local_size);
  // for (int i = 0; i < local_size; i++) {
  //   Set *t = &transactions[i];
  //   printf("\nItemset (size %d): ", t->size);
  //   for (int j = 0; j < t->size; j++) {
  //     printf(" %c ", index_to_char(t->set[j]));
  //   }
  // }
  // printf("\n");

  // compute first iteration in serial ??? No, i think that should already be
  // parallelized
  charm(transactions, local_size, 3);

  // compute charm_extend() in parallel

  // how to gather the results later? MPI_Gather I suppose ...

  free(transactions);

  end_time = MPI_Wtime();
  total_time = end_time - start_time;
  printf("Process %d: Execution time = %f seconds\n", rank, total_time);

  MPI_Finalize();

  return 0;
}
