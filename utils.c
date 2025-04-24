#include "utils.h"
#include "set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare(const void *a, const void *b) { return *(int *)a - *(int *)b; }

int char_to_index(char c) { return c - 'a'; }

char index_to_char(int i) { return i + 'a'; }

int count_lines_in_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  int lines = 0;
  char buffer[MAX_LINE_LENGTH];

  while (fgets(buffer, sizeof(buffer), file)) {
    bool new_line = false;
    for (int i = 0; buffer[i] != '\0'; i++) {
      if (buffer[i] == '\n') {
        lines++;
        new_line = true;
      }
    }
    if (!new_line) {
      perror("Line exceeds maximum line length\n");
      exit(EXIT_FAILURE);
    }
  }

  fclose(file);
  return lines;
}

Set *read_sets_from_file_start_end(const char *filename, int *num_transactions,
                                   int rank, int size, int *partition_size,
                                   int *local_size, bool characters, int max_transactions) {
  *num_transactions = count_lines_in_file(filename);
  if (rank == 0) {
    printf("Number of transactions in the dataset: %d\n", *num_transactions);
  }
  if (*num_transactions > max_transactions) {
    // printf("Max number of transactions exceeded. Considering only first %d transactions\n", max_transactions);
    *num_transactions = max_transactions;
  }

  *partition_size = *num_transactions / size;
  *local_size = (rank == size - 1)
                    ? *num_transactions - *partition_size * (size - 1)
                    : *partition_size;

  Set *transactions = malloc(*local_size * sizeof(Set));
  if (!transactions) {
    perror("Failed to allocate memory for transactions");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open file");
    free(transactions);
    exit(EXIT_FAILURE);
  }

  char line[MAX_LINE_LENGTH];
  int start_index = rank * *partition_size;
  int end_index = start_index + *local_size;
  int transaction_index = 0, line_index = -1;

  while (fgets(line, sizeof(line), file) && start_index < end_index) {
    line_index++;
    if (line_index < start_index) {
      continue;
    }
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }
    Set *current_set = &transactions[transaction_index++];
    set_init(current_set, len / 2);

    char *token = strtok(line, " ");
    while (token && *token != '\n') {
      if (characters) {
        int elem = -1;
        if (strlen(token) == 1 && token[0] >= 'a' && token[0] <= 'z') {
          // to save memory, we can convert the char in an int
          elem = char_to_index(token[0]);
        } else {
          printf("Item cannot be loaded. It will not be added to the set\n");
        }
        set_add(current_set, elem);
      } else {
        int elem = atoi(token);
        set_add(current_set, elem);
      }
      token = strtok(NULL, " ");
    }
    start_index++;
  }

  fclose(file);
  return transactions;
}

Set *read_sets_from_file(const char *filename, int *num_transactions,
                         bool characters, int max_transactions) {
  *num_transactions = count_lines_in_file(filename);
  // printf("Number of transactions in the dataset: %d\n", *num_transactions);
  if (*num_transactions > max_transactions) {
    // printf("Max number of transactions exceeded\n");
    *num_transactions = max_transactions;
  }

  Set *transactions = malloc(*num_transactions * sizeof(Set));
  if (!transactions) {
    // perror("Failed to allocate memory for transactions");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open file");
    free(transactions);
    exit(EXIT_FAILURE);
  }

  char line[MAX_LINE_LENGTH];
  int index = 0;

  while (fgets(line, sizeof(line), file) && index < *num_transactions) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }

    Set *current_set = &transactions[index];
    set_init(current_set, len / 2);

    char *token = strtok(line, " ");
    while (token && *token != '\n') {
      if (characters) {
        int elem = -1;
        if (strlen(token) == 1 && token[0] >= 'a' && token[0] <= 'z') {
          // to save memory, we can convert the char in an int
          elem = char_to_index(token[0]);
        } else {
          printf("Item cannot be loaded. It will not be added to the set\n");
        }
        set_add(current_set, elem);
      } else {
        int elem = atoi(token);
        set_add(current_set, elem);
      }
      token = strtok(NULL, " ");
    }

    index++;
  }

  fclose(file);
  return transactions;
}
