#include "charm_functions.h"
#include "itarray.h"
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
    fprintf(stderr, "Data path and file must be defined!");
    return -1;
  } else {
    asprintf(&data_path_file, "%s/%s", data_path, data_file);
  }

  char *env_char = getenv("DATA_CHARACTERS");
  bool characters = true;
  if (env_char != NULL) {
    characters = atoi(env_char) != 0;
  }

  char *env_min_support = getenv("MIN_SUPPORT");
  int min_support = -1;
  if (env_min_support == NULL) {
    fprintf(stderr, "Min support is missing but required!");
    return -1;
  } else {
    min_support = atoi(env_min_support);
  }

  int num_transactions = 0, partition_size = 0, local_size = 0;
  Set *transactions = read_sets_from_file_start_end(
      data_path_file, &num_transactions, rank, size,
      &partition_size, &local_size, characters);

  int local_min_support = min_support / size;

  int tid_start = rank * partition_size + 1;
  ITArray C = charm(transactions, local_size, local_min_support, tid_start);

  free(transactions);
  
  print_closed_itemsets(&C, true);

  int *buffer, bufsize;
  serialize_itarray(&C, &buffer, &bufsize);

  ITArray deserialized = {{0}};
  int *des_buffer = (int *)malloc(bufsize * sizeof(int));
  deserialize_itarray(buffer, &deserialized);

  end_time = MPI_Wtime();
  total_time = end_time - start_time;
  printf("Process %d took %f seconds\n", rank, total_time);

  MPI_Finalize();

  return 0;
}
