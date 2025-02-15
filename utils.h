#pragma once
#include "set.h"
#include <stdbool.h>

/******************** CONSTANTS ********************/
#define MAX_LINE_LENGTH 400

/******************** METHODS ********************/

int compare(const void *a, const void *b);
int char_to_index(char c);
char index_to_char(int i);
int count_lines_in_file(const char *filename);
Set *read_sets_from_file_start_end(const char *filename, int *num_transactions,
                                   int rank, int size, int *partition_size,
                                   int *local_size, bool characters, int max_transactions);
Set *read_sets_from_file(const char *filename, int *num_transactions,
                         bool characters, int max_transactions);
