#include "charm_functions.h"
#include "itarray.h"
#include "utils.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {

  int rank = 0, size = 0;
  MPI_Init(NULL, NULL);

  double start_time = 0, end_time = 0, total_time = 0;

  start_time = MPI_Wtime();

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  printf("My rank is %d/%d\n", rank, size);

  char *data_path = getenv("DATA_PATH");
  char *data_file = getenv("DATA_FILE");
  char data_path_file[1000];

  if (data_path == NULL || data_file == NULL) {
    fprintf(stderr, "Data path and file must be defined!");
    MPI_Abort(MPI_COMM_WORLD, -1);
  } else {
    sprintf(data_path_file, "%s/%s", data_path, data_file);
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
    MPI_Abort(MPI_COMM_WORLD, -1);
  } else {
    min_support = atoi(env_min_support);
  }

  int num_transactions = 0, partition_size = 0, local_size = 0;
  Set *transactions = read_sets_from_file_start_end(
      data_path_file, &num_transactions, rank, size, &partition_size,
      &local_size, characters);

  int local_min_support = min_support / size;

  int tid_start = rank * partition_size + 1;
  ITArray local_C =
      charm(transactions, local_size, local_min_support, tid_start);

  for (int i = 0; i < local_size; i++) {
    set_free(&transactions[i]);
  }
  free(transactions);

  if (rank + 1 <= size / 2) {
    int num_senders = (rank + 1) * 2 == size ? 1 : 2;
    printf("Rank %d will receive %d messages\n", rank, num_senders);
    int *buffers[2] = {0};
    for (int i = 0; i < num_senders; i++) {
      MPI_Status status;
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      int message_size;
      MPI_Get_count(&status, MPI_INT, &message_size);
      buffers[i] = calloc(message_size, sizeof(int));
      if (buffers[i] == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
      }
      MPI_Recv(buffers[i], message_size, MPI_INT, status.MPI_SOURCE,
               status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("Rank %d received message from rank %d\n", rank,
             status.MPI_SOURCE);
      ITArray sent;
      deserialize_itarray(buffers[i], &sent);
      merge_closed_itemsets_into(&sent, &local_C, true);
      itarray_free(&sent);
      free(buffers[i]);
    }
  } else {
    printf("Rank %d will not receive a message\n", rank);
  }

  if (rank != 0) {
    int *buffer, bufsize;
    serialize_itarray(&local_C, &buffer, &bufsize);
    int target_rank = (rank - 1) / 2;
    printf("Rank %d is sending message to rank %d\n", rank, target_rank);
    MPI_Send(buffer, bufsize, MPI_INT, target_rank, 0, MPI_COMM_WORLD);
    free(buffer);
  } else {
    itarray_remove_low_suport_pairs(&local_C, min_support);
    itarray_remove_subsumed_pairs(&local_C);
    print_closed_itemsets(&local_C, characters);
  }

  end_time = MPI_Wtime();
  total_time = end_time - start_time;
  printf("Process %d took %f seconds\n", rank, total_time);
  itarray_free(&local_C);
  MPI_Finalize();

  return 0;
}
