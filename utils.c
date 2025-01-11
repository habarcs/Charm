#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare(const void *a, const void *b) { return *(int *)a - *(int *)b; }

void out_of_bounds(int i) {
  if (i >= ARRAY_SIZE) {
    fprintf(stderr, "Array out of bounds, exiting\n");
    abort();
  }
}

Set set_union(Set a, Set b) {
  Set uni = {0};
  int i, j;
  i = j = 0;
  while (i < a.size && j < b.size) {
    out_of_bounds(uni.size);
    if (a.set[i] < b.set[j]) {
      uni.set[uni.size] = a.set[i];
      uni.size++;
      i++;
    } else if (a.set[i] > b.set[j]) {
      uni.set[uni.size] = b.set[j];
      uni.size++;
      j++;
    } else {
      uni.set[uni.size] = a.set[i];
      uni.size++;
      i++;
      j++;
    }
  }
  for (; i < a.size; i++) {
    out_of_bounds(uni.size);
    uni.set[uni.size++] = a.set[i];
  }
  for (; j < b.size; j++) {
    out_of_bounds(uni.size);
    uni.set[uni.size++] = b.set[j];
  }
  return uni;
}

Set set_intersect(Set a, Set b) {
  Set inter = {0};
  int i, j;
  i = j = 0;
  while (i < a.size && j < b.size) {
    if (a.set[i] < b.set[j]) {
      i++;
    } else if (a.set[i] > b.set[j]) {
      j++;
    } else {
      out_of_bounds(inter.size);
      inter.set[inter.size] = a.set[i];
      inter.size++;
      i++;
      j++;
    }
  }
  return inter;
}

bool is_subset(Set sub, Set sup) {
  int i, j;
  i = j = 0;
  while (i < sub.size && j < sup.size) {
    if (sub.set[i] < sup.set[j]) {
      return false;
    } else if (sub.set[i] > sup.set[j]) {
      j++;
    } else {
      i++;
      j++;
    }
  }
  if (i < sub.size) {
    return false;
  }
  return true;
}

bool sets_equal(Set a, Set b) {
  if (a.size != b.size) {
    return false;
  }
  for (int i = 0; i < a.size; i++) {
    if (a.set[i] != b.set[i]) {
      return false;
    }
  }
  return true;
}

int compare_sets(Set a, Set b) {
  int i = 0;
  while (i < a.size && i < b.size) {
    if (a.set[i] < b.set[i]) {
      return -1;
    } else if (a.set[i] > b.set[i]) {
      return 1;
    }
    i++;
  }
  if (i < a.size) {
    return -1;
  }
  if (i < b.size) {
    return 1;
  }
  return 0;
}

int compare_itpairs(const void *a, const void *b) {
  return compare_sets(((ITPair *)a)->itemset, ((ITPair *)b)->itemset);
}

void add_itemset_if_not_subsumed(ITArray *C, ITPair itpair) {
  for (int i = 0; i < C->size; i++) {
    if (C->itpairs[i].tidset.size == itpair.tidset.size &&
        is_subset(itpair.itemset, C->itpairs[i].itemset)) {
      return;
    }
  }
  out_of_bounds(C->size);
  C->itpairs[C->size++] = itpair;
}

void remove_itpair(ITArray *P, int pos) {
  if (pos >= P->size) {
    return;
  }
  for (int i = pos; i < P->size - 1; i++) {
    P->itpairs[i] = P->itpairs[i + 1];
  }
  P->size--;
}

void replace_with(ITArray *P, Set it, Set with) {
  for (int i = 0; i < P->size; i++) {
    if (is_subset(it, P->itpairs[i].itemset)) {
      P->itpairs[i].itemset = set_union(P->itpairs[i].itemset, with);
    }
  }
}

// we can convert back to char the items
void print_closed_itemsets(ITArray C, bool character) {
  printf("Closed itemsets found:\n");
  for (int i = 0; i < C.size; i++) {
    printf("itemset: ");
    for (int j = 0; j < C.itpairs[i].itemset.size; j++) {
      if (character) {
        printf("%c ", index_to_char(C.itpairs[i].itemset.set[j]));
      } else {
        printf("%d ", C.itpairs[i].itemset.set[j]);
      }
    }
    printf("\t| tids: ");
    for (int j = 0; j < C.itpairs[i].tidset.size; j++) {
      printf("%d ", C.itpairs[i].tidset.set[j]);
    }
    printf("\n");
  }
}

int char_to_index(char c) {
  return c - 'a';
}

char index_to_char(int i) {
  return i + 'a';
}

int count_lines_in_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  int lines = 0;
  char buffer[MAX_LINE_LENGTH];

  while (fgets(buffer, sizeof(buffer), file)) {
    lines++;
  }

  fclose(file);
  return lines;
}

Set *read_sets_from_file_start_end(const char *filename, int *num_transactions, int rank, int size, int *partition_size, int *local_size, bool characters) {
  *num_transactions = count_lines_in_file(filename);
  printf("Number of transactions in the dataset: %d\n", *num_transactions);
  if (*num_transactions > MAX_TRANSACTIONS) {
    printf("Max number of transactions exceeded\n");
    *num_transactions = MAX_TRANSACTIONS;
  }

  *partition_size = *num_transactions / size;
  *local_size = (rank == size - 1) ? *num_transactions - *partition_size * (size - 1) : *partition_size;

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
    current_set->size = 0;

    char *token = strtok(line, " ");
    while (token && *token != '\n') {
      if (strlen(token) == 1) {
        int elem = -1;
        if (characters && token[0] >= 'a' && token[0] <= 'z') {
          // to save memory, we can convert the char in an int
          elem = char_to_index(token[0]);
        } else if (!characters) {
          elem = token[0];
        } else {
          printf("Item was not a char neither an int. It will not be added to the set\n");
        }
        current_set->set[current_set->size++] = elem;
      }
      token = strtok(NULL, " ");
    }
    start_index++;
  }

  fclose(file);
  return transactions;
}

Set *read_sets_from_file(const char *filename, int *num_transactions, bool characters) {
  *num_transactions = count_lines_in_file(filename);
  printf("Number of transactions in the dataset: %d\n", *num_transactions);
  if (*num_transactions > MAX_TRANSACTIONS) {
    printf("Max number of transactions exceeded\n");
    *num_transactions = MAX_TRANSACTIONS;
  }

  Set *transactions = malloc(*num_transactions * sizeof(Set));
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
  int index = 0;

  while (fgets(line, sizeof(line), file) && index < *num_transactions) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }

    Set *current_set = &transactions[index];
    current_set->size = 0;

    char *token = strtok(line, " ");
    while (token && *token != '\n') {
      if (strlen(token) == 1) {
        int elem = -1;
        if (characters && token[0] >= 'a' && token[0] <= 'z') {
          // to save memory, we can convert the char in an int
          elem = char_to_index(token[0]);
        } else if (!characters) {
          elem = token[0];
        } else {
          printf("Item was not a char neither an int. It will not be added to the set\n");
        }
        current_set->set[current_set->size++] = elem;
      }
      token = strtok(NULL, " ");
    }

    index++;
  }

  fclose(file);
  return transactions;
}
